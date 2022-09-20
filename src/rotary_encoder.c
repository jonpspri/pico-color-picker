/*
 * SPDX-FileCopyrightText: 2022 Jonathan Springer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later

 * This file is part of pico-color-picker.
 *
 * pico-color-picker is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * pico-color-picker is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * pico-color-picker. If not, see <https://www.gnu.org/licenses/>.
 *
 * Portions of this program are part of MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include "pico/stdlib.h"

#include "hardware/pio.h"

#include "pio_irq.h"
#include "rotary_encoder.h"
#include "pico_debug.h"

#include "rotary_encoder.pio.h"

#define PIOx __CONCAT(pio, ROTARY_ENCODER_PIO)
#define GPIO_FUNC_PIOx __CONCAT(GPIO_FUNC_PIO, ROTARY_ENCODER_PIO)

typedef struct {
  uint16_t rx;
  uint16_t idx;
  int8_t delta;
  int8_t sub_count;
} transition_history_t;
uint8_t transition_history_idx;
transition_history_t transition_history[256];

uint32_t prior_state;

typedef struct {
  bool inverted;
  int8_t sub_count;
  rotary_encoder_callback_function callback;
  transition_history_t *transition_history;
} rotary_encoder_info;

static rotary_encoder_info rotary_encoders[NUM_PIO_STATE_MACHINES];
static uint8_t active_encoders_bitmask = 0u;

static const int8_t transitions[16] = {
        0,    // 0 00 -> 00 no movement
        -1,   // 1 00 -> 01 3/4 ccw
        +1,   // 2 00 -> 10 3/4 cw
        0,    // 3 00 -> 11 non-Gray-code transition
        +1,   // 4 01 -> 00 2/4 cw
        0,    // 5 01 -> 01 no movement
        0,    // 6 01 -> 10 non-Gray-code transition
        -1,   // 7 01 -> 11 4/4 ccw
        -1,   // 8 10 -> 00 2/4 ccw
        0,    // 9 10 -> 01 non-Gray-code transition
        0,    // A 10 -> 10 no movement
        +1,   // B 10 -> 11 4/4 cw
        0,    // C 11 -> 00 non-Gray-code transition
        +1,   // D 11 -> 01 1/4 cw
        -1,   // E 11 -> 10 1/4 ccw
        0,    // F 11 -> 11 no movement
};

static void rotary_encoder_interrupt_handler(PIO pio, uint8_t sm, void *filler) {

  while(pio_sm_get_rx_fifo_level(pio, sm)) {
    uint16_t rx16 = pio_sm_get(pio, sm);
    transition_history[transition_history_idx].rx = (uint16_t)rx16;
    /*
     * Step 1 - decipher the bits coming from the PIO
     */
    for(int i=0; i<4; i++) {
      rotary_encoder_info *re = &rotary_encoders[i];
      if(!re->callback) { continue; }

      uint16_t rx = rx16>>(i*2);
      uint8_t prior_state, new_state;
      if (re->inverted) {
        prior_state = (rx & 0x2u)>>1 | (rx & 0x1u)<<1;  /* X...XB'A' -> A'B' */
        new_state = (rx & 0x100u)>>7 | (rx & 0x200u)>>9;    /* BAX..X -> AB */
      } else {
        prior_state = rx & 0x3u;                        /* X..XA'B' -> A'B' */
        new_state = (rx & 0x300u)>>8;                     /* ABX..X -> AB */
      }
      if(new_state == prior_state) continue;

      /*
       * Step 2 - construct an index to the callback table
       */
      uint32_t idx = prior_state<<2 | new_state;

      /*
       * Step 3 - adjust the sub-count (between-detent clicks) and callback if we hit a detent
       */
      re->sub_count += transitions[idx];
      if (re->sub_count >= ROTARY_ENCODER_DIVISOR) {
        re->callback(i, 1);
        re->sub_count = 0;
      } else if (re->sub_count <= ROTARY_ENCODER_DIVISOR * -1) {
        re->callback(i, -1);
        re->sub_count = 0;
      }
      re->transition_history[transition_history_idx].rx = rx;
      re->transition_history[transition_history_idx].idx = idx;
      re->transition_history[transition_history_idx].delta = transitions[idx];
      re->transition_history[transition_history_idx].sub_count = re->sub_count;
      transition_history_idx++;
    }
  }
}

void rotary_encoder_register_encoder(
    uint8_t re_number,
    bool inverted,
    rotary_encoder_callback_function callback
) {
  rotary_encoders[re_number].inverted = inverted;
  rotary_encoders[re_number].callback = callback;
  rotary_encoders[re_number].transition_history = calloc(sizeof(transition_history_t), 256);
}

void rotary_encoder_init_encoders(uint8_t low_pin) {
  for(uint8_t re=0; re<4; re++) {
    if(!rotary_encoders[re].callback) continue;
    for(uint8_t i=0; i<2; i++) {
      uint8_t pin = ROTARY_ENCODER_LOW_PIN + re*2 + i;
      gpio_set_function(pin, GPIO_FUNC_PIOx);
      gpio_set_input_enabled(pin, true);
      gpio_pull_up(pin);
    }
  }
  pio_sm_claim(PIOx, ROTARY_ENCODER_SM);
  uint8_t program_offset = pio_add_program(PIOx, &rotary_encoder_program);
  rotary_encoder_program_init(PIOx, ROTARY_ENCODER_SM, program_offset, ROTARY_ENCODER_LOW_PIN);

  pio_sm_exec(PIOx, ROTARY_ENCODER_SM, 0xe040);  /* set y, 0 */
  pio_sm_exec(PIOx, ROTARY_ENCODER_SM, 0xa04a);  /* mov y, !y - make y 0xffff - guarantee a PUSH */
  pio_sm_set_enabled(PIOx, ROTARY_ENCODER_SM, true);

  /*  A moment of truth -- filling y should ensure that there is some data in the RX FIFO.  */
  prior_state = pio_sm_get_blocking(PIOx, ROTARY_ENCODER_SM);

  /*  Now that the pump is primed, it's time to enable the interrupts for the
   *  PIO.  */
  pio_irq_set_handler(PIOx, ROTARY_ENCODER_SM,
      rotary_encoder_interrupt_handler, NULL,
      PIO_INTR_SM0_RXNEMPTY_BITS);
}

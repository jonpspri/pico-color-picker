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

#ifndef ROTARY_ENCODER_PIO
#define ROTARY_ENCODER_PIO 1
#endif

#ifndef ROTARY_ENCODER_DIVISOR
#define ROTARY_ENCODER_DIVISOR 4
#endif

#define PIOx __CONCAT(pio, ROTARY_ENCODER_PIO)
#define GPIO_FUNC_PIOx __CONCAT(GPIO_FUNC_PIO, ROTARY_ENCODER_PIO)

typedef struct {
  uint8_t rx;
  uint8_t idx;
  int8_t delta;
  int8_t sub_count;
} transition_history_t;

typedef struct {
  const char *name;
  bool inverted;
  uint32_t prior_state;  /* Use 32 bits to ease integration with the PIO RX FIFO */
  int8_t sub_count;
  rotary_encoder_callback_function callback;
  uint8_t transition_history_idx;
  transition_history_t *transition_history;
} rotary_encoder_info;

static uint8_t program_offset = 0xffu;
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

static void rotary_encoder_interrupt_handler(PIO pio, uint8_t sm, void *re_in) {
  rotary_encoder_info *re = (rotary_encoder_info *)re_in;

  while(pio_sm_get_rx_fifo_level(pio, sm)) {
    /*
     * Step 1 - decipher the 4 bits coming from the PIO
     */
    uint32_t rx = pio_sm_get(pio, sm);   /*  Arrives as ABB'A' (or BAA'B') */
    uint8_t prior_state, new_state;
    if (re->inverted) {
      prior_state = (rx & 0x3u)>>1 | (rx & 0x1u)<<1;  /* XXB'A' -> A'B' */
      new_state = (rx & 0x8u)>>3 | (rx & 0x4u)>>1;    /* BAXX -> AB */
    } else {
      prior_state = rx & 0x3u;                        /* XXA'B' -> A'B' */
      new_state = (rx & 0xCu)>>2;                     /* ABXX -> AB */
    }

    /*
     * Step 2 - construct an index to the callback table
     */
    assert(new_state != prior_state);  /*  The PIO should filter this case out already */
    uint32_t idx = prior_state<<2 | new_state;

    /*
     * Step 3 - adjust the sub-count (between-detent clicks) and callback if we hit a detent
     */
    re->sub_count += transitions[idx];
    if (re->sub_count >= ROTARY_ENCODER_DIVISOR) {
      re->callback(sm, 1);
      re->sub_count = 0;
    } else if (re->sub_count <= ROTARY_ENCODER_DIVISOR * -1) {
      re->callback(sm, -1);
      re->sub_count = 0;
    }
    re->transition_history[re->transition_history_idx].rx = rx;
    re->transition_history[re->transition_history_idx].idx = idx;
    re->transition_history[re->transition_history_idx].delta = transitions[idx];
    re->transition_history[re->transition_history_idx].sub_count = re->sub_count;
    re->transition_history_idx++;
  }
}

void rotary_encoder_init(
    uint8_t re_number,
    const char *name,
    uint8_t pin,
    bool inverted,
    rotary_encoder_callback_function callback
) {
  assert(re_number < NUM_PIO_STATE_MACHINES);
  assert(pin < 31);  /* Should probably be <21, but I can't quite bring myself to do that... */

  for(uint8_t i=0; i<2; i++) {
    gpio_set_function(pin+i, GPIO_FUNC_PIOx);
    gpio_set_input_enabled(pin+i, true);
    gpio_pull_up(pin+i);
  }

  pio_sm_claim(PIOx, re_number);

  if (program_offset >= 32) {
    program_offset = pio_add_program(PIOx, &rotary_encoder_program);
  }

  rotary_encoders[re_number].name = name;
  rotary_encoders[re_number].inverted = inverted;
  rotary_encoders[re_number].prior_state = ( gpio_get_all() >> pin ) & 3u;
  rotary_encoders[re_number].callback = callback;
  rotary_encoders[re_number].transition_history_idx = 0;
  rotary_encoders[re_number].transition_history = calloc(sizeof(transition_history_t),256);

  rotary_encoder_program_init(PIOx, re_number, program_offset, pin);
  active_encoders_bitmask |= 1u << re_number;

  /*  A moment of truth -- moving 31 into y should ensure that there is some
   *  data in the RX FIFO.  */
  pio_sm_exec(PIOx, re_number, 0xe05f);  /* set y, 31 - guarantee a first push */
  pio_sm_set_enabled(PIOx, re_number, true);
  debug_printf("State Machine %d initialized... getting first item.", re_number);
  rotary_encoders[re_number].prior_state = pio_sm_get(PIOx, re_number);
  debug_printf("Received value %lx from State Machine %d", rotary_encoders[re_number].prior_state, re_number);

  /*  Now that the pump is primed, it's time to enable the interrupts for the
   *  PIO.  */
  pio_irq_set_handler(PIOx, re_number,
      rotary_encoder_interrupt_handler, &rotary_encoders[re_number],
      PIO_INTR_SM0_RXNEMPTY_BITS);
  debug_printf("IRQ Handler set up for State Machine %d", re_number);
}

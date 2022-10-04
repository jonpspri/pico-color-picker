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

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include <stdlib.h>
#include "pico/stdlib.h"

#include "hardware/pio.h"

#include "io_devices.h"
#include "log.h"

#include "io_devices.pio.h"

#define PIOx __CONCAT(pio, IO_DEVICES_PIO)
#define GPIO_FUNC_PIOx __CONCAT(GPIO_FUNC_PIO, IO_DEVICES_PIO)

typedef void (*irq_interrupt_handler_type)(PIO, uint8_t, TaskHandle_t);
static irq_interrupt_handler_type _interrupt_handler[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static void *_interrupt_arg[NUM_PIOS][NUM_PIO_STATE_MACHINES];

static uint8_t io_devices_8_offset = 32;

typedef struct {
  bool enabled;
} button_info;

static button_info buttons[8];

typedef struct {
  uint32_t rx;
  uint16_t idx;
  int8_t delta;
  int8_t sub_count;
} transition_history_t;
uint8_t transition_history_idx;

typedef struct {
  bool inverted;
  int8_t sub_count;
  bool enabled;
  transition_history_t *transition_history;
} rotary_encoder_info;

static rotary_encoder_info rotary_encoders[4];

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

__isr static void pio_irq_interrupt_handler(void) {
    uint8_t i = 1;
    PIO pio = PIOx;
    for (uint8_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
      if (!_interrupt_handler[i][sm]) continue;
      uint32_t intf = (PIO_INTR_SM0_RXNEMPTY_BITS|PIO_INTR_SM0_TXNFULL_BITS|PIO_INTR_SM0_BITS)<<sm;
      if (pio->ints0 & intf) _interrupt_handler[i][sm](pio, sm, _interrupt_arg[i][sm]);
    }
}

__isr static void button_interrupt_handler(PIO pio, uint8_t sm, TaskHandle_t task_to_signal) {
  uint32_t notify_bits = 0u;
  while(pio_sm_get_rx_fifo_level(pio, sm)) {
    uint32_t pio_data = pio_sm_get(pio, sm);

    for(int i=0; i<8; i++) {
      if (!buttons[i].enabled) continue;

      uint8_t prior_state, new_state;
      prior_state = (pio_data >> i) & 0x1u;
      new_state = (pio_data >> (8+i)) & 0x1u;

      if(new_state ^ prior_state) {
        uint32_t notify_temp = 1u;
        notify_temp |= new_state ? 0 : 2u;
        notify_bits |= notify_temp << (i*2);
      }
    }
  }

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyIndexedFromISR(task_to_signal, 1,
      notify_bits,
      eSetBits,
      &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

__isr static void rotary_encoder_interrupt_handler(PIO pio, uint8_t sm, TaskHandle_t task_to_signal) {
  uint32_t notify_bits = 0u;

  while(pio_sm_get_rx_fifo_level(pio, sm)) {
    uint32_t pio_rx = pio_sm_get(pio, sm);

    /*
     * Step 1 - decipher the bits coming from the PIO (Rotary Encoders)
     */
    for(int i=0; i<4; i++) {
      uint16_t rx = pio_rx >> (i*2);
      uint8_t prior_state, new_state;
      rotary_encoder_info *re = &rotary_encoders[i];

      if (!re->enabled) continue;

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
        /* debug_printf("RE %d +1 (index %d)", i, transition_history_idx); */
        notify_bits |= 1 << (i*2);
        re->sub_count = 0;
      } else if (re->sub_count <= ROTARY_ENCODER_DIVISOR * -1) {
        /* debug_printf("RE %d -1 (index %d)", i, transition_history_idx); */
        notify_bits |= 1 << (i*2+1);
        re->sub_count = 0;
      }
      re->transition_history[transition_history_idx].rx = rx;
      re->transition_history[transition_history_idx].idx = idx;
      re->transition_history[transition_history_idx].delta = transitions[idx];
      re->transition_history[transition_history_idx].sub_count = re->sub_count;
      transition_history_idx++;
    }
  }

  if (notify_bits) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyIndexedFromISR(task_to_signal, 1,
        notify_bits,
        eSetBits,
        &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

void pio_irq_set_handler(PIO pio, uint8_t sm, irq_interrupt_handler_type handler, TaskHandle_t arg, int mask) {
  uint8_t i = pio_get_index(pio);
  irq_set_enabled(PIO0_IRQ_0 + i*2, false);

  uint32_t inte = pio->inte0;
  inte &= ~((PIO_INTR_SM0_RXNEMPTY_BITS|PIO_INTR_SM0_TXNFULL_BITS|PIO_INTR_SM0_BITS)<<sm);
  inte |= mask << sm;
  pio->inte0 = inte;

  _interrupt_arg[i][sm] = arg;
  _interrupt_handler[i][sm] = handler;

  irq_set_exclusive_handler(PIO0_IRQ_0 + i*2, pio_irq_interrupt_handler);
  irq_set_enabled(PIO0_IRQ_0 + i*2, true);
}

static uint8_t program_offset() {
  if (io_devices_8_offset < 32) return io_devices_8_offset;
  return (io_devices_8_offset = pio_add_program(PIOx, &io_devices_8_program));
}

static void init_sm(uint8_t low_pin, uint8_t sm) {
  pio_sm_claim(PIOx, sm);
  io_devices_8_program_init(PIOx, sm, program_offset(), low_pin);

  pio_sm_exec(PIOx, sm, 0xe040);  /* set y, 0 */
  pio_sm_exec(PIOx, sm, 0xa04a);  /* mov y, !y - make y 0xffff - guarantee a PUSH */
  pio_sm_set_enabled(PIOx, sm, true);
}

static void init_pin(uint8_t pin) {
      gpio_set_function(pin, GPIO_FUNC_PIOx);
      gpio_set_input_enabled(pin, true);
      gpio_pull_up(pin);
}

void io_devices_register_encoder( uint8_t re_number, bool inverted) {
  rotary_encoders[re_number].inverted = inverted;
  rotary_encoders[re_number].enabled = true;
  rotary_encoders[re_number].transition_history =
    memset(pvPortMalloc(sizeof(transition_history_t)*256),0,sizeof(transition_history_t)*256);
}

void io_devices_register_button( uint8_t button_number) {
  buttons[button_number].enabled = true;
}

void io_devices_init_encoders(uint8_t low_pin, uint8_t sm) {
  log_trace("Initializing encoders on SM %d", sm);
  for(uint8_t re=0; re<4; re++) {
    if(!rotary_encoders[re].enabled) continue;
    for(uint8_t i=0; i<2; i++) init_pin(low_pin + re*2 + i);
  }
  init_sm(low_pin, sm);
  pio_irq_set_handler(PIOx, sm,
      rotary_encoder_interrupt_handler, xTaskGetCurrentTaskHandle(),
      PIO_INTR_SM0_RXNEMPTY_BITS);
}

void io_devices_init_buttons(uint8_t low_pin, uint8_t sm) {
  log_trace("Initializing buttons on SM %d", sm);
  for(uint8_t b=0; b<8; b++) {
    if(buttons[b].enabled) init_pin(low_pin + b);
  }
  init_sm(low_pin, sm);
  pio_irq_set_handler(PIOx, sm,
      button_interrupt_handler, xTaskGetCurrentTaskHandle(),
      PIO_INTR_SM0_RXNEMPTY_BITS);
}

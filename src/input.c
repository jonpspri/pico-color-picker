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

#include "context.h"
#include "input.h"
#include "log.h"

#include "io_devices.pio.h"

#define PIOx __CONCAT(pio, IO_DEVICES_PIO)
#define GPIO_FUNC_PIOx __CONCAT(GPIO_FUNC_PIO, IO_DEVICES_PIO)

static irq_interrupt_handler_type _interrupt_handler[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static void *_interrupt_arg[NUM_PIOS][NUM_PIO_STATE_MACHINES];

static uint8_t io_devices_8_offset = 32;

static uint8_t program_offset() {
  if (io_devices_8_offset < 32) return io_devices_8_offset;
  return (io_devices_8_offset = pio_add_program(PIOx, &io_devices_8_program));
}

__isr static void pio_irq_interrupt_handler(void) {
    uint8_t i = 1;
    PIO pio = PIOx;
    for (uint8_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
      if (!_interrupt_handler[i][sm]) continue;
      uint32_t intf = (PIO_INTR_SM0_RXNEMPTY_BITS|PIO_INTR_SM0_TXNFULL_BITS|PIO_INTR_SM0_BITS)<<sm;
      if (pio->ints0 & intf) _interrupt_handler[i][sm](pio, sm, _interrupt_arg[i][sm]);
    }
}

void input_pio_irq_set_handler(PIO pio, uint8_t sm, irq_interrupt_handler_type handler, TaskHandle_t arg, int mask) {
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

void input_init_sm(uint8_t low_pin, uint8_t sm) {
  pio_sm_claim(PIOx, sm);
  io_devices_8_program_init(PIOx, sm, program_offset(), low_pin);

  pio_sm_exec(PIOx, sm, 0xe040);  /* set y, 0 */
  pio_sm_exec(PIOx, sm, 0xa04a);  /* mov y, !y - make y 0xffff - guarantee a PUSH */
  pio_sm_set_enabled(PIOx, sm, true);
}

void input_init_pin(uint8_t pin) {
      gpio_set_function(pin, GPIO_FUNC_PIOx);
      gpio_set_input_enabled(pin, true);
      gpio_pull_up(pin);
}

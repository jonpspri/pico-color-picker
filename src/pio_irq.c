/*
 * SPDX-FileCopyrightText: 2022 Jonathan Springer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
 * Portions of this file copied from MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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
#include "hardware/pio.h"
#include "hardware/sync.h"

#include "pio_irq.h"

static PIO pio_instances[NUM_PIOS] = { pio0, pio1 };
static irq_interrupt_handler_type _interrupt_handler[NUM_PIOS][NUM_PIO_STATE_MACHINES];
static void *_interrupt_arg[NUM_PIOS][NUM_PIO_STATE_MACHINES];

static void pio_irq_interrupt_handler(void) {
    uint8_t i = 1;
    PIO pio = pio1;
    for (uint8_t sm = 0; sm < NUM_PIO_STATE_MACHINES; sm++) {
      if (!_interrupt_handler[i][sm]) continue;
      uint32_t intf = (PIO_INTR_SM0_RXNEMPTY_BITS|PIO_INTR_SM0_TXNFULL_BITS|PIO_INTR_SM0_BITS)<<sm;
      if (pio->ints0 & intf) _interrupt_handler[i][sm](pio, sm, _interrupt_arg[i][sm]);
    }
}

void pio_irq_set_handler(PIO pio, uint8_t sm, irq_interrupt_handler_type handler, void *arg, int mask) {
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

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
 */

#ifndef __INPUT_H
#define __INPUT_H

#include "FreeRTOS.h"
#include "task.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*irq_interrupt_handler_type)(PIO, uint8_t, TaskHandle_t);

void input_init_pin(uint8_t pin);
void input_init_sm(uint8_t low_pin, uint8_t sm);
void input_pio_irq_set_handler(PIO pio, uint8_t sm, irq_interrupt_handler_type handler, TaskHandle_t arg, int mask);

#ifdef __cplusplus
}
#endif

#endif

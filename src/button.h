/* vim: set ft=cpp:
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

#ifndef __BUTTONS_H
#define __BUTTONS_H

#include "pico/stdlib.h"

#include "context.h"

extern uint8_t buttons;
extern uint8_t buttons_depressed;

inline bool button_depressed_p(uint8_t index) { assert(index<8); return buttons_depressed & (1<<index); }

void button_task(void *parm);
void button_init(uint8_t pin, uint8_t sm);
void button_return_callback(context_t *c, void *data, v32_t value);

#endif /* __BUTTON_H */

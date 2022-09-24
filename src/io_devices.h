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

#ifndef __ROTARY_ENCODER_H
#define __ROTARY_ENCODER_H

#include "pico/stdlib.h"

void io_devices_register_encoder(uint8_t re_number, bool inverted);
void io_devices_init_encoders(uint8_t pin, uint8_t sm);
void io_devices_register_button(uint8_t button_number);
void io_devices_init_buttons(uint8_t pin, uint8_t sm);

#endif

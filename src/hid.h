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

#ifndef __HID_H
#define __HID_H

#include "pico/stdlib.h"

typedef uint32_t hid_handle;

typedef enum {
  HID_ROTARY_ENCODER,
  HID_BUTTON
} hid_t;

typedef enum {
  HID_ROTARY_ENCODER_CW,
  HID_ROTARY_ENCODER_CCW,
  HID_BUTTON_PRESS,
  HID_BUTTON_RELEASE
} hid_event_t;

typedef struct {
  hid_t hid_type;
  uint32_t hid_component_handle;
  hid_event_t hid_event;
} hid_event_rec;

extern int32_t hid_encoder_value(hid_handle handle);
extern void hid_encoder_init(uint pin_a, uint pin_b, hid_handle *handle);
extern void hid_start_polling();
#endif

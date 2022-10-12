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

/** @file note_color.h
 */

#include "pico/stdlib.h"

#ifndef __NOTE_COLOR_H
#define __NOTE_COLOR_H

#define COLORS_COUNT 12

#ifdef __cplusplus
extern "C" {
#endif

typedef struct note_color note_color_t;

const char *note_color_note_name_i(uint8_t i);
const char *note_color_note_name(note_color_t *n);
uint32_t *note_color_rgb_i(uint8_t i);
uint32_t *note_color_rgb(note_color_t *n);
note_color_t *note_color_ptr_i(uint8_t i);
void note_color_init();

#ifdef __cplusplus
}
#endif

#endif

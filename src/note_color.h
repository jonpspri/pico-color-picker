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

#include "context.h"

#ifndef __NOTE_COLOR_H
#define __NOTE_COLOR_H

#define COLORS_COUNT 12

#ifdef __cplusplus
extern "C" {
#endif

typedef struct note_color note_color_t;

void note_color_init();
context_t *note_color_chord_alloc();
context_t *note_color_menu_alloc();

#ifdef __cplusplus
}
#endif

#endif

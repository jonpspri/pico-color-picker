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

#ifndef __MENU_H
#define __MENU_H

#include "pico/stdlib.h"

#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t magic_number;
  bool selectable;
  bool selected;
  context_t *enter_context;
  void *data;
} menu_item_t;

typedef struct {
  uint32_t magic_number;
  uint8_t cursor_at;
  uint8_t item_count;
  context_callback_table_t callbacks;
  void (*render_item)(menu_item_t *item, bitmap_t *b, uint8_t);
  menu_item_t *items;
} menu_t;

bool menu_context_init(context_t *c, context_t *parent, menu_t *menu, context_leds_t *);

#ifdef __cplusplus
}
#endif

#endif

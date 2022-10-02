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

#ifndef __MENU_CONTEXT_H
#define __MENU_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "context.h"

typedef void format_data(char *buffer, uint8_t buffer_size, void *data);

typedef struct menu_item {
  bool has_checkbox;
  bool checked;
  context_handle_t enter_context;   /* TODO: Flesh out what I need to do this */
  bitmap_handle_t bitmap;
  size_t data_size;
  void *data;
} menu_item_t;

typedef struct menu_context_data {
  uint32_t cursor_location;
  uint32_t menu_items_max;
  menu_item_t *menu_items;
} menu_context_data_t;

extern void menu_context_enable(context_handle_t *menu_context);
extern context_handle_t *menu_context_init(menu_item_t *menu_items, uint32_t menu_items_max);

#ifdef __cplusplus
}
#endif

#endif

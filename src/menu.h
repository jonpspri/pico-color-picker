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

typedef struct menu_item menu_item_t;
typedef struct menu menu_t;

/* Menu builder functions */
void menu_builder_init(uint8_t cursors, uint8_t items);
void menu_builder_set_selection_changed_cb(void ( * )(menu_t *) );
void menu_builder_set_render_item_cb(void ( * )(menu_item_t *,
        bitmap_t *,
        uint8_t
        ) );
void menu_builder_set_item_enter_ctx(uint8_t item, context_t *ctx);
void menu_builder_set_item_data(uint8_t item, void *data);
void menu_builder_set_item_string(uint8_t item, const char *data);
void menu_builder_set_cursor_enter_data(uint8_t cursor, void *data);
menu_t *menu_builder_menu_ptr();

context_t *menu_builder_finalize();

menu_item_t *menu_item_alloc(context_t *enter_context, void *enter_data,
        void *data);

void *menu_item_data(menu_item_t *mi);

uint8_t menu_cursor_at(menu_t *, uint8_t cursor);
menu_item_t *menu_item_at_cursor(menu_t *, uint8_t cursor, int8_t offset);
void menu_item_render_string(menu_item_t *item, bitmap_t *item_bitmap, uint8_t cursor);

#ifdef __cplusplus
}
#endif

#endif /* ifndef __MENU_H */

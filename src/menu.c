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

#include "context.h"
#include "menu.h"

static context_screen_t cs;
static bitmap_t item_bitmap;

static void menu_re_callback(void *data, v32_t v) {
  menu_t *menu = (menu_t *) data;
  menu->cursor_at += v.s;
  menu->cursor_at = MAX(menu->cursor_at, 0);
  menu->cursor_at = MIN(menu->cursor_at, menu->menu_item_count-1);
  menu->screen_at = MIN(menu->screen_at, menu->cursor_at);
  menu->screen_at = MAX(menu->screen_at, menu->cursor_at-2);
}

static void menu_ui_callback(context_t *c, void *data, v32_t v) {
  menu_t *menu = (menu_t *) data;

  xSemaphoreTake(cs.mutex, portMAX_DELAY);
  context_screen_set_button_char(&cs, 0, c->parent ? LAQUO : 32);
  context_screen_set_button_char(&cs, 1, menu->items[menu->cursor_at].context ? RAQUO : 32);
  for(uint8_t i=0; i<3; i++) {
    bitmap_clear(&item_bitmap);
    uint8_t idx = i+menu->screen_at;
    menu->render_item(&menu->items[idx], &item_bitmap, i);
    bitmap_copy_from(&cs.pane, &item_bitmap, 0, i*cs.pane.height/3);
  }
  xTaskNotifyIndexed(tasks.screen, 1, (uint32_t)&cs, eSetValueWithOverwrite);
  xSemaphoreGive(cs.mutex);
}

bool menu_context_init(context_t *c, context_t *parent, menu_t *menu) {
  context_screen_set_re_label(&cs, 0, "Up/Down");
  bitmap_init(&item_bitmap, cs.pane.width, cs.pane.height/3, NULL);

  menu->callbacks.re_handlers[ROTARY_ENCODER_RED_OFFSET].callback=menu_re_callback;
  menu->callbacks.re_handlers[ROTARY_ENCODER_RED_OFFSET].data=menu;
  menu->callbacks.ui_update.callback=menu_ui_callback;
  return context_init(c, parent, &menu->callbacks, &cs, &menu);
}

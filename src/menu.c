/*
 * SPDX-FileCopyrightText: 2022 Jonathan Springer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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

#include "button.h"
#include "context.h"
#include "menu.h"

/* -------------------- Callbacks -------------------- */

static void menu_re_callback(context_t *c, void *data, v32_t v) {
  menu_t *menu = (menu_t *) data;
  menu->cursor_at = (menu->cursor_at + menu->item_count + v.s) % menu->item_count;
  menu->selection_changed_cb(menu);
}

static void menu_ui_callback(context_t *c, void *data, v32_t v) {
  static bitmap_t item_bitmap;
  menu_t *menu = (menu_t *) data;
  ASSERT_IS_A(menu, MENU_T);

  bitmap_init(&item_bitmap, c->screen->pane.width, c->screen->pane.height/3, NULL);

  xSemaphoreTake(c->screen->mutex, portMAX_DELAY);
  context_screen_set_button_char(c->screen, 0, (context_stack_depth() > 1) ? LAQUO : 32);
  context_screen_set_button_char(c->screen, 1, menu->items[menu->cursor_at].enter_context ? RAQUO : 32);

  /*  NOTE the render_item calls should also change the LED value */
  uint8_t offset = (menu->cursor_at + menu->item_count - 1) % menu->item_count;
  bitmap_clear(&item_bitmap);
  menu->render_item_cb(&menu->items[offset], &item_bitmap);
  bitmap_copy_from(&c->screen->pane, &item_bitmap, 0, 0);

  bitmap_clear(&item_bitmap);
  offset = (offset + 1) % menu->item_count;
  menu->render_item_cb(&menu->items[offset], &item_bitmap);
  bitmap_invert(&item_bitmap);
  bitmap_copy_from(&c->screen->pane, &item_bitmap, 0, c->screen->pane.height/3);

  bitmap_clear(&item_bitmap);
  offset = (offset + 1) % menu->item_count;
  menu->render_item_cb(&menu->items[offset], &item_bitmap);
  bitmap_copy_from(&c->screen->pane, &item_bitmap, 0, 2*c->screen->pane.height/3);

  xSemaphoreGive(c->screen->mutex);
}

static void button_forward_callback(context_t* c, void *data, v32_t value) {
  menu_t *m = (menu_t *)data;
  ASSERT_IS_A(m, MENU_T);

  menu_item_t *mi=&m->items[m->cursor_at];

  if (mi->forward_cb) mi->forward_cb(mi);
  if (value.u) context_push(m->items[m->cursor_at].enter_context);
}

/* ---------------------------------------------------------------------- */

void menu_init(context_t *c, menu_t *menu, context_leds_t *leds) {
  c->screen = pcp_zero_malloc(sizeof(context_screen_t));
  context_screen_init(c->screen);
  context_screen_set_re_label(c->screen, 0, "Up/Down");

  menu->callbacks.re[ROTARY_ENCODER_RED_OFFSET].callback=menu_re_callback;
  menu->callbacks.re[ROTARY_ENCODER_RED_OFFSET].data=menu;

  menu->callbacks.button[BUTTON_LOWER_OFFSET].callback=button_forward_callback;
  menu->callbacks.button[BUTTON_LOWER_OFFSET].data=menu;

  menu->callbacks.screen.callback=menu_ui_callback;
  menu->callbacks.screen.data=menu;

  menu->selection_changed_cb(menu);

  context_init(c, &menu->callbacks, c->screen, leds, &menu);
}

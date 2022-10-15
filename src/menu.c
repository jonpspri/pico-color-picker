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

#include "pico/stdlib.h"

#include "pcp.h"

#include "button.h"
#include "context.h"
#include "menu.h"

/* -------------------- Structs -------------------- */

struct menu_item {
  pcp_t pcp;
  context_t *enter_context;
  void *enter_data;
  void *data;
};

struct menu {
  pcp_t pcp;
  uint8_t cursor_at;
  uint8_t item_count;
  menu_item_t **items;
  void (*selection_changed_cb)(menu_t *menu);
  void (*render_item_cb)(menu_item_t *item, bitmap_t *b);
};

/* -------------------- Callbacks -------------------- */

static void s_menu_re_callback(context_t *c, void *data, v32_t v) {
  menu_t *menu = (menu_t *) data;
  menu->cursor_at = (menu->cursor_at + menu->item_count + v.s) % menu->item_count;
  menu->selection_changed_cb(menu);
}

static void s_menu_ui_callback(context_t *c, void *data, v32_t v) {
  menu_t *menu = (menu_t *) data;
  ASSERT_IS_A(menu, MENU_T);

  bitmap_t *pane = context_get_drawing_pane(c);
  bitmap_t *item_bitmap = bitmap_alloc(pane->width, pane->height/3, NULL);

  context_set_upper_button_char(c, (context_stack_depth() > 1) ? LAQUO : 32);
  context_set_lower_button_char(c, menu->items[menu->cursor_at]->enter_context ? RAQUO : 32);

  /*  NOTE the render_item calls should also change the LED value */
  uint8_t offset = (menu->cursor_at + menu->item_count - 1) % menu->item_count;
  bitmap_clear(item_bitmap);
  menu->render_item_cb(menu->items[offset], item_bitmap);
  bitmap_copy_from(pane, item_bitmap, 0, 0);

  bitmap_clear(item_bitmap);
  offset = (offset + 1) % menu->item_count;
  menu->render_item_cb(menu->items[offset], item_bitmap);
  bitmap_invert(item_bitmap);
  bitmap_copy_from(pane, item_bitmap, 0, pane->height/3);

  bitmap_clear(item_bitmap);
  offset = (offset + 1) % menu->item_count;
  menu->render_item_cb(menu->items[offset], item_bitmap);
  bitmap_copy_from(pane, item_bitmap, 0, 2*pane->height/3);

  bitmap_free(item_bitmap);
}

static void s_button_forward_callback(context_t* c, void *data, v32_t value) {
  menu_t *m = (menu_t *)data;
  ASSERT_IS_A(m, MENU_T);

  if (!value.u) return;
  log_trace("About to push context, data:  %lx %lx",
      m->items[m->cursor_at]->enter_data,
      m->items[m->cursor_at]->enter_context);
  context_push(m->items[m->cursor_at]->enter_context, m->items[m->cursor_at]->enter_data);
}

/* ---------------------------------------------------------------------- */
static void s_menu_item_free(void *v) {
  menu_item_t *mi = (menu_item_t *)v;
  ASSERT_IS_A(mi, MENU_ITEM_T);
  if(mi->data) pcp_free(mi->data);
  vPortFree(v);
}

menu_item_t *menu_item_alloc(context_t *enter_context, void *enter_data, void *data) {
  menu_item_t *mi = pcp_zero_malloc(sizeof(menu_item_t));
  mi->pcp.magic_number = MENU_ITEM_T;
  mi->pcp.free_f = s_menu_item_free;
  mi->pcp.autofree_p = true;
  mi->enter_context = enter_context;
  mi->enter_data = enter_data;
  mi->data = data;
  return mi;
}

/* ---------------------------------------------------------------------- */

void *menu_item_data(menu_item_t *mi) {
  return mi->data;
}

uint8_t menu_cursor_at(menu_t *m) {
  return m->cursor_at;
}

/* ---------------------------------------------------------------------- */

void s_menu_free(void *v) {
  menu_t *m = (menu_t *)v;
  ASSERT_IS_A(m, MENU_T);

  for (uint8_t i=0; i<m->item_count; i++) {
    if (m->items[i]) pcp_free(m->items[i]);
  }
  vPortFree(v);
}

void menu_builder_init() {
  menu_t *m = pcp_zero_malloc(sizeof(menu_t));
  m->pcp.magic_number = MENU_T;
  m->pcp.free_f = s_menu_free;
  m->pcp.autofree_p = true;

  context_builder_init();
  context_builder_set_red_re(s_menu_re_callback, m, NULL, NULL, "Up/Down");
  context_builder_set_lower_button(s_button_forward_callback, m, RAQUO);
  context_builder_set_display_callback(s_menu_ui_callback, m);

  vTaskSetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU, m);
}

void menu_builder_set_items(menu_item_t **items, uint8_t count) {
  menu_t *m = (menu_t *)pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU);
  ASSERT_IS_A(m, MENU_T);
  m->items = items;
  m->item_count = count;
}

void menu_builder_set_selection_changed_cb(void (*f)(menu_t *)) {
  menu_t *m = (menu_t *)pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU);
  ASSERT_IS_A(m, MENU_T);
  m->selection_changed_cb = f;
}

void menu_builder_set_render_item_cb(void (*f)(menu_item_t *, bitmap_t *)) {
  menu_t *m = (menu_t *)pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU);
  ASSERT_IS_A(m, MENU_T);
  m->render_item_cb = f;
}

context_t *menu_builder_finalize() {
  menu_t *m = (menu_t *)pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU);
  ASSERT_IS_A(m, MENU_T);
  if (m->selection_changed_cb) m->selection_changed_cb(m);
  vTaskSetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU, NULL);
  return context_builder_finalize();
}

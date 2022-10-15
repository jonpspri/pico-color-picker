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
  uint32_t magic_number;
  context_t *enter_ctx;
  void *enter_data;
  void *data;
};

#define CURSOR_MAX 3
#define ITEM_MAX 12

typedef struct cursor cursor_t;
struct cursor {
  uint32_t magic_number;
  menu_t *menu;  /*  I hate that this needs to be here, but it simplifies callback  */
  uint8_t cursor_at;
};

struct menu {
  pcp_t pcp;
  uint8_t cursor_count;
  cursor_t cursors[CURSOR_MAX];
  uint8_t item_count;
  menu_item_t items[ITEM_MAX];
  void (*selection_changed_cb)(menu_t *menu);
  void (*render_item_cb)(menu_item_t *item, bitmap_t *b);
};

static const uint8_t re_offsets[] = {
  RE_RED_OFFSET,
  RE_GREEN_OFFSET,
  RE_BLUE_OFFSET
};

static const uint8_t re_button_offsets[] = {
  BUTTON_RED_OFFSET,
  BUTTON_GREEN_OFFSET,
  BUTTON_BLUE_OFFSET
};
/* -------------------- Callbacks -------------------- */

static void s_menu_re_callback(context_t *c, void *data, v32_t v) {
  cursor_t *cursor = (cursor_t *)data;
  ASSERT_IS_A(cursor, CURSOR_T);
  menu_t *menu = cursor->menu;

  cursor->cursor_at = (cursor->cursor_at + menu->item_count + v.s) % menu->item_count;
  menu->selection_changed_cb(menu);
}

static void s_menu_ui_callback(context_t *c, void *data, v32_t v) {
  menu_t *menu = (menu_t *) data;
  ASSERT_IS_A(menu, MENU_T);

  bitmap_t *pane = context_get_drawing_pane(c);
  bitmap_t *item_bitmap = bitmap_alloc(pane->width, pane->height/3, NULL);

  context_set_upper_button_char(c, (context_stack_depth() > 1) ? LAQUO : 32);
  context_set_lower_button_char(c, menu->items[menu->cursors[0].cursor_at].enter_ctx ? RAQUO : 32);

  uint8_t offset = (menu->cursors[0].cursor_at + menu->item_count - 1) % menu->item_count;
  bitmap_clear(item_bitmap);
  menu->render_item_cb(&menu->items[offset], item_bitmap);
  bitmap_copy_from(pane, item_bitmap, 0, 0);

  bitmap_clear(item_bitmap);
  offset = (offset + 1) % menu->item_count;
  menu->render_item_cb(&menu->items[offset], item_bitmap);
  bitmap_invert(item_bitmap);
  bitmap_copy_from(pane, item_bitmap, 0, pane->height/3);

  bitmap_clear(item_bitmap);
  offset = (offset + 1) % menu->item_count;
  menu->render_item_cb(&menu->items[offset], item_bitmap);
  bitmap_copy_from(pane, item_bitmap, 0, 2*pane->height/3);

  bitmap_free(item_bitmap);
}

static void s_button_forward_callback(context_t* c, void *data, v32_t value) {
  cursor_t *cursor = (cursor_t *)data;
  ASSERT_IS_A(cursor, CURSOR_T);
  menu_t *m = cursor->menu;

  if (!value.u) return;
  log_trace("About to push context, data:  %lx %lx",
      m->items[cursor->cursor_at].enter_data,
      m->items[cursor->cursor_at].enter_ctx);
  context_push(m->items[cursor->cursor_at].enter_ctx, m->items[cursor->cursor_at].enter_data);
}

/* ---------------------------------------------------------------------- */

void *menu_item_data(menu_item_t *mi) {
  return mi->data;
}

uint8_t menu_cursor_at(menu_t *m, uint8_t cursor) {
  assert(cursor < m->cursor_count);
  return m->cursors[cursor].cursor_at;
}

menu_item_t *menu_item_at_cursor(menu_t *m, uint8_t cursor, int8_t offset) {
  assert(cursor < m->cursor_count);
  return &m->items[(menu_cursor_at(m, cursor) + m->item_count + offset) % m->item_count];
}

/* ---------------------------------------------------------------------- */

void s_menu_free(void *v) {
  menu_t *m = (menu_t *)v;
  ASSERT_IS_A(m, MENU_T);

  for (uint8_t i=0; i<m->item_count; i++) {
    if (m->items[i].enter_data) pcp_free(m->items[i].enter_data);
    if (m->items[i].data) pcp_free(m->items[i].data);
  }
  vPortFree(v);
}

void menu_builder_init(uint8_t cursors, uint8_t items) {
  assert(cursors <= CURSOR_MAX);
  assert(cursors == 1); /* For now */
  assert(items <= ITEM_MAX);

  menu_t *m = pcp_zero_malloc(sizeof(menu_t));
  m->pcp.magic_number = MENU_T;
  m->pcp.free_f = s_menu_free;
  m->pcp.autofree_p = true;

  m->cursor_count = cursors;
  m->item_count = items;

  context_builder_init();

  char label[16];
  for (uint8_t i=0; i<cursors; i++) {
    m->cursors[i].magic_number = CURSOR_T;
    m->cursors[i].menu = m;

    snprintf(label, 15, "Cursor %1d", i);
    context_builder_set_re(re_offsets[i], re_button_offsets[i],
        s_menu_re_callback, &m->cursors[i],
        s_button_forward_callback, &m->cursors[i],
        label);
  }

  if (m->cursor_count == 1)  {
    context_builder_set_lower_button(s_button_forward_callback, &m->cursors[0], RAQUO);
  };

  for (uint8_t i=0; i<items; i++) {
    m->items[i].magic_number = MENU_ITEM_T;
  }

  context_builder_set_display_callback(s_menu_ui_callback, m);

  vTaskSetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU, m);
}

void menu_builder_set_item_enter_ctx(uint8_t item, context_t *ctx, void *data) {
  menu_t *m = (menu_t *)pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU);
  ASSERT_IS_A(m, MENU_T);
  assert(item < m->item_count);

  m->items[item].enter_ctx = ctx;
  m->items[item].enter_data = data;
}

void menu_builder_set_item_data(uint8_t item, void *data) {
  menu_t *m = (menu_t *)pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU);
  ASSERT_IS_A(m, MENU_T);
  assert(item < m->item_count);

  m->items[item].data = data;
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

menu_item_t *menu_builder_get_item_ptr(uint8_t item) {
  menu_t *m = (menu_t *)pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU);
  ASSERT_IS_A(m, MENU_T);
  assert(item < m->item_count);
  return &m->items[item];
}

context_t *menu_builder_finalize() {
  menu_t *m = (menu_t *)pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU);
  ASSERT_IS_A(m, MENU_T);
  if (m->selection_changed_cb) m->selection_changed_cb(m);
  vTaskSetThreadLocalStoragePointer(NULL, ThLS_BLDR_MENU, NULL);
  return context_builder_finalize();
}

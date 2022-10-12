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

/** @file color_menu.c
 *
 *  Mange the colors menu.  Currently behaves as a singleton object, assuming there
 *  is only one color main menu in the structure.
 */

#include "context.h"
#include "menu.h"
#include "note_color.h"
#include "rgb_encoder.h"

static context_leds_t rgb_ptrs;
static menu_item_t color_items[12];
static menu_t colors_menu;

/* ---------------------------------------------------------------------- */

static void menu_render_item_callback(menu_item_t *item, bitmap_t *item_bitmap) {
  char buffer[25];
  note_color_t *nc = (note_color_t *)item->data;
  ASSERT_IS_A(nc, NOTE_COLOR_T);

  bitmap_clear(item_bitmap);
  sprintf(buffer, "%-5s #%06lx", note_color_note_name(nc), *note_color_rgb(nc));
  bitmap_draw_string(item_bitmap, 8, 0, &TRIPLE_LINE_TEXT_FONT, buffer);

  /* TO DO:  This doesn't really belong here.  There should be a separate selection changed callback. */
  /* rgb_ptrs.rgb_p[line_number] = &nc->rgb; */

  if(!item->selectable) return;
  if(item->selected) {
    bitmap_draw_square(item_bitmap, 0, 1, 4, 5);
  } else {
    bitmap_draw_empty_square(item_bitmap, 0, 1, 4, 5);
  }
}

static void s_line1_render_callback(context_t *c, void *data, v32_t v) {
  menu_item_t *item = (menu_item_t *)data;
  ASSERT_IS_A(item, MENU_ITEM_T);

  rgb_encoders_data_t *re = (rgb_encoders_data_t *)item->enter_context->data;
  assert(re->magic_number == RGB_ENCODERS_DATA_T);

  menu_render_item_callback(item, &c->screen->pane);
}

static void selection_changed_callback(menu_t *menu) {
  rgb_ptrs.magic_number = CONTEXT_LEDS_T;
  rgb_ptrs.rgb_p[0] = note_color_rgb_i((menu->cursor_at + menu->item_count - 1) % menu->item_count);
  rgb_ptrs.rgb_p[1] = note_color_rgb_i(menu->cursor_at);
  rgb_ptrs.rgb_p[2] = note_color_rgb_i((menu->cursor_at + 1) % menu->item_count);
}

static menu_item_t *color_menu_items(context_t *c) {
#ifndef NDEBUG
  /*  Validate that the initialize code is only called once */
  static bool initialize_called;
  assert(!initialize_called);
  initialize_called = true;
#endif

  note_color_init();
  for (uint8_t i=0; i<12; i++) {
    color_items[i].magic_number = MENU_ITEM_T;
    color_items[i].selectable = false;
    color_items[i].selected = 0;
    color_items[i].enter_context = pcp_zero_malloc(sizeof(context_t));
    rgb_encoder_init(color_items[i].enter_context, c, note_color_rgb_i(i));
    color_items[i].enter_context->callbacks->line1.callback=s_line1_render_callback;
    color_items[i].enter_context->callbacks->line1.data=&color_items[i];
    color_items[i].data = note_color_ptr_i(i);
  }

  return color_items;
}

void colors_menu_init(context_t *c, context_t *parent) {
  assert(!colors_menu.items); /* This function should be called only once */

  colors_menu.magic_number = MENU_T;
  colors_menu.item_count = 12;
  colors_menu.render_item_cb = menu_render_item_callback;
  colors_menu.selection_changed_cb = selection_changed_callback;
  colors_menu.items = color_menu_items(c);

  menu_init(c, parent, &colors_menu, &rgb_ptrs);
}

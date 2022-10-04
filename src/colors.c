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

#include <stdio.h>

#include "pico/stdlib.h"

#include "bitmap.h"
#include "context.h"

static struct {
  char *note_name;
  uint32_t rgb;
} note_colors[] = {
  { "C", 0xFF0000 },
  { "C#/Db", 0xcc1100 },
  { "D", 0xbb2200 },
  { "D#/Eb", 0xcc5500 },
  { "E", 0xffcc00 },
  { "F", 0x33ff00 },
  { "F#/Gb", 0x00cd71 },
  { "G", 0x008AA1 },
  { "G#/Ab", 0x2161b0 },
  { "A", 0x2200ff },
  { "A#/B#", 0x860e90 },
  { "B", 0xB8154A }
};

typedef struct colors_menu_item {
  bool checked;
  context_t *context;
  uint32_t rgb;
} colors_menu_item_t;

typedef struct colors_menu {
  int8_t cursor_at;
  colors_menu_item_t colors[12];
} colors_menu_t;

static context_callback_table_t colors_callbacks;
static colors_menu_t colors_menu;

static void colors_menu_re_callback(void *data, v32_t v) {
  int8_t idx = colors_menu.cursor_at;
  idx += v.s;
  idx = MAX(idx, 0);
  idx = MIN(idx, 11);
  colors_menu.cursor_at = idx;
}

static void colors_menu_ui_callback(void *data, v32_t v) {
  static context_screen_t *cs;
  static bitmap_t *item_bitmap;
  static char buffer[25];
  static uint8_t screen_at = 0;

  if (!cs) {
    cs = context_screen_init();
    context_screen_set_re_label(cs, 0, "Up/Down");
    context_screen_set_button_char(cs, 0, 32);
    context_screen_set_button_char(cs, 1, RAQUO);
  }

  if (!item_bitmap) {
    item_bitmap = bitmap_init(cs->pane->width, cs->pane->height/3, NULL);
  }

  screen_at = MIN(screen_at, colors_menu.cursor_at);
  screen_at = MAX(screen_at, colors_menu.cursor_at-2);

  bitmap_clear(cs->pane);
  for(uint8_t i=0; i<3; i++) {
    bitmap_clear(item_bitmap);
    uint8_t idx = i+screen_at;
    sprintf(buffer, "%-5s #%06lx", note_colors[idx].note_name, colors_menu.colors[idx].rgb);
    bitmap_draw_empty_square(item_bitmap, 0, 1, 4, 5);
    bitmap_draw_string(item_bitmap, 8, 0, &TRIPLE_LINE_TEXT_FONT, buffer);
    if (idx == colors_menu.cursor_at) bitmap_invert(item_bitmap);
    bitmap_copy_from(cs->pane, item_bitmap, 0, i*cs->pane->height/3);
  }

  xTaskNotifyIndexed(tasks.screen, 1, (uint32_t)cs, eSetValueWithOverwrite);
  xTaskNotifyIndexed(tasks.leds, 1, colors_menu.colors[colors_menu.cursor_at].rgb, eSetValueWithOverwrite);
}

void colors_context_enable(context_t *context) {
  if(tasks.rotary_encoders)
    xTaskNotifyIndexed(tasks.rotary_encoders, 2, (uint32_t)&colors_callbacks, eSetValueWithOverwrite);
  if(tasks.buttons)
    xTaskNotifyIndexed(tasks.buttons, 2, (uint32_t)&colors_callbacks, eSetValueWithOverwrite);
}

context_t *colors_context_init() {
  for (uint8_t i=0; i<12; i++) {
    colors_menu.colors[i].rgb = note_colors[i].rgb;
  }

  colors_callbacks.re_handlers[ROTARY_ENCODER_RED_OFFSET].callback=colors_menu_re_callback;
  colors_callbacks.ui_update.callback=colors_menu_ui_callback;

  return context_init(&colors_callbacks, sizeof(colors_menu), &colors_menu);
}

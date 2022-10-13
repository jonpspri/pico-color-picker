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

/** @file note_color.c
 *
 *  @brief Manager the notes to colors mapping, both "permanent" and transient as it
 *         is manipulated through the color menu and through the chord editor.
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "pcp.h"
#include "log.h"

#include "button.h"
#include "context.h"
#include "note_color.h"
#include "menu.h"

/* ---------------------------------------------------------------------- */

/** @brief Manage the "color" of a particular note in the 12-note space.  */
struct note_color {
  uint32_t magic_number;
  const char *note_name;
  uint32_t rgb;
  context_t *rgb_encoder_ctx;
};

typedef struct {
  uint32_t magic_number;
  bool active;
  uint8_t value;
  uint8_t shift;
  uint8_t button_offset;
} rgb_encoder_t;

typedef struct rgb_encoders_data {
  uint32_t magic_number;
  SemaphoreHandle_t rgb_encoder_mutex;
  uint32_t *rgb;
  context_callback_table_t callbacks;
  rgb_encoder_t rgb_encoders[IO_PIO_SLOTS/2];  /*  We waste storage to simplify lookup.  Maybe not necessary with callbacks?  */
} rgb_encoders_data_t;

typedef struct rgb_encoder_frame {
  pcp_t pcp;
  void (*line1)(void *);
  void *ref;
} rgb_encoder_frame_t;

/* ---------------------------------------------------------------------- */

static const char *initial_names[12] = {
  "C", "C#/Db", "D", "D#/Eb", "E", "F",
  "F#/Gb", "G", "G#/Ab", "A", "A#/B#", "B"
};

static uint32_t initial_rgbs[12] = {
0xFF0000 , 0xcc1100 , 0xbb2200 , 0xcc5500 , 0xffcc00 , 0x33ff00 ,
0x00cd71 , 0x008AA1 , 0x2161b0 , 0x2200ff , 0x860e90 , 0xB8154A
};

/* ---------------------------------------------------------------------- */

static note_color_t note_colors[12];
static context_screen_t cs;

static context_leds_t rgb_ptrs;
static menu_item_t color_items[12];
static menu_t colors_menu;

/* ---------------------------------------------------------------------- */

static rgb_encoder_frame_t *rgb_encoder_frame_alloc(void (*line1)(void *), void *ref) {
  rgb_encoder_frame_t *f = pcp_zero_malloc(sizeof(rgb_encoder_frame_t));
  f->pcp.magic_number = RGB_ENCODER_FRAME_T | FREEABLE_P;
  f->pcp.free = vPortFree;
  f->line1 = line1;
  f->ref = ref;
  return f;
}

static uint32_t s_rgb_encoders_value(rgb_encoders_data_t *re) {
  uint32_t rgb=0u;
  xSemaphoreTake(re->rgb_encoder_mutex, portMAX_DELAY);
  for (int i=0; i<4; i++) {
    if (!re->rgb_encoders[i].active) continue;
    rgb |= re->rgb_encoders[i].value << re->rgb_encoders[i].shift;
  }
  xSemaphoreGive(re->rgb_encoder_mutex);
  return rgb;
}

static void s_rgb_encoders_re_callback(context_t *c, void *re_v, v32_t delta) {
  rgb_encoder_t *re = (rgb_encoder_t *)re_v;
  assert(re->magic_number == RGB_ENCODER_T);

  log_trace("RGB Encoder initial value %02x", re->value);

  int16_t value = re->value;
  int8_t multiplier = button_depressed_p(re->button_offset) ? 0x1u : 0x11u;
  value += delta.s * multiplier;
  value = MAX(value, 0);
  value = MIN(value, 0xff);
  re->value = value;

  rgb_encoders_data_t *red = (rgb_encoders_data_t *)c->data;
  ASSERT_IS_A(red, RGB_ENCODERS_DATA_T);
  *red->rgb = s_rgb_encoders_value((rgb_encoders_data_t *)c->data);

  log_trace("RGB Encoder new value %02x", re->value);
};

static void s_display_callback(context_t *c, void *data, v32_t v) {
  static char hex_color_value[8];
  rgb_encoder_frame_t *f = NULL;

  log_trace("Entering RGB Encoder s_display_callback");

  if (c == context_current()) {
    f = (rgb_encoder_frame_t *)context_frame_data();
    if(f) ASSERT_IS_A(f, RGB_ENCODER_FRAME_T);
  } else {
    f = NULL;
    log_warn("Context mismatch: %lx vs %lx", c, context_current());
  }

  rgb_encoders_data_t *re = ((rgb_encoders_data_t *) c->data);
  assert(re->magic_number == RGB_ENCODERS_DATA_T);

  log_trace("-- UI Decoder with REs %02x %02x %02x",
      re->rgb_encoders[ROTARY_ENCODER_RED_OFFSET].value,
      re->rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].value,
      re->rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].value);

  sprintf(hex_color_value, "#%06lx", *re->rgb);

  if (f) f->line1(f->ref);
  bitmap_draw_string(&c->screen->pane, 0, TRIPLE_LINE_TEXT_FONT.Height, &DOUBLE_LINE_TEXT_FONT, hex_color_value);
}

static context_t *s_rgb_encoder_init(uint32_t *rgb) {
  rgb_encoders_data_t *rgb_encoders=pcp_zero_malloc(sizeof(struct rgb_encoders_data));
  rgb_encoders->magic_number = RGB_ENCODERS_DATA_T;

  rgb_encoders->rgb_encoder_mutex=xSemaphoreCreateMutex();
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_RED_OFFSET].magic_number = RGB_ENCODER_T;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_RED_OFFSET].active = true;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_RED_OFFSET].shift = 16;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_RED_OFFSET].button_offset = BUTTON_RED_OFFSET;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_RED_OFFSET].value = *rgb >> 16 & 0xff;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].magic_number = RGB_ENCODER_T;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].active = true;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].shift = 8;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].button_offset = BUTTON_GREEN_OFFSET;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].value = *rgb >> 8 & 0xff;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].magic_number = RGB_ENCODER_T;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].active = true;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].shift = 0;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].button_offset = BUTTON_BLUE_OFFSET;
  rgb_encoders->rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].value = *rgb & 0xff;

  rgb_encoders->rgb=rgb;

  rgb_encoders->callbacks.re[ROTARY_ENCODER_RED_OFFSET].callback=s_rgb_encoders_re_callback;
  rgb_encoders->callbacks.re[ROTARY_ENCODER_RED_OFFSET].data=&rgb_encoders->rgb_encoders[ROTARY_ENCODER_RED_OFFSET];
  rgb_encoders->callbacks.re[ROTARY_ENCODER_GREEN_OFFSET].callback=s_rgb_encoders_re_callback;
  rgb_encoders->callbacks.re[ROTARY_ENCODER_GREEN_OFFSET].data=&rgb_encoders->rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET];
  rgb_encoders->callbacks.re[ROTARY_ENCODER_BLUE_OFFSET].callback=s_rgb_encoders_re_callback;
  rgb_encoders->callbacks.re[ROTARY_ENCODER_BLUE_OFFSET].data=&rgb_encoders->rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET];

  rgb_encoders->callbacks.button[BUTTON_UPPER_OFFSET].callback=button_return_callback;

  rgb_encoders->callbacks.screen.callback=s_display_callback;
  rgb_encoders->callbacks.screen.data=(void *)rgb_encoders;

  context_screen_init(&cs);
  context_screen_set_re_label(&cs, 0, "Red");
  context_screen_set_re_label(&cs, 1, "Green");
  context_screen_set_re_label(&cs, 2, "Blue");
  cs.button_chars[0] = LAQUO;
  cs.button_chars[1] = 32;

  return context_alloc(&rgb_encoders->callbacks, &cs, rgb_encoders);
}


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

static void s_line1_render_callback(void *data) {
  menu_item_t *item = (menu_item_t *)data;
  ASSERT_IS_A(item, MENU_ITEM_T);

  menu_render_item_callback(item, &context_current()->screen->pane);
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
    color_items[i].enter_context = note_colors[i].rgb_encoder_ctx;
    color_items[i].enter_data = rgb_encoder_frame_alloc(s_line1_render_callback, &color_items[i]);
    color_items[i].data = note_color_ptr_i(i);
  }

  return color_items;
}

static void s_color_menu_entry(context_t *c, void *data, v32_t v) {
  xTaskNotifyIndexed(tasks.display, NTFCN_IDX_LEDS, (uint32_t)&rgb_ptrs, eSetValueWithOverwrite);
}

/* ---------------------------------------------------------------------- */

void note_color_menu_init(context_t *c) {
  assert(!colors_menu.items); /* This function should be called only once */

  colors_menu.magic_number = MENU_T;
  colors_menu.item_count = 12;
  colors_menu.render_item_cb = menu_render_item_callback;
  colors_menu.selection_changed_cb = selection_changed_callback;
  colors_menu.items = color_menu_items(c);

  menu_init(c, &colors_menu, &s_color_menu_entry);
}

const char *note_color_note_name_i(uint8_t i) {
  assert(i<12);
  return note_colors[i].note_name;
}

const char *note_color_note_name(note_color_t *n) {
  return n->note_name;
}

uint32_t *note_color_rgb_i(uint8_t i) {
  assert(i<12);
  return &note_colors[i].rgb;
}

uint32_t *note_color_rgb(note_color_t *n) {
  return &n->rgb;
}

note_color_t *note_color_ptr_i(uint8_t i) {
  assert(i<12);
  return &note_colors[i];
}

void note_color_init() {
  for(uint8_t i=0; i<12; i++) {
    note_colors[i].magic_number = NOTE_COLOR_T;
    note_colors[i].note_name = initial_names[i];
    note_colors[i].rgb = initial_rgbs[i];
    note_colors[i].rgb_encoder_ctx = s_rgb_encoder_init(&note_colors[i].rgb);
  }
}

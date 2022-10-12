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
#include "note_color.h"
#include "context.h"

#define CELL_WIDTH (RE_LABEL_TOTAL_WIDTH/3)

typedef struct chord chord_t;
struct chord {
  uint32_t magic_number;
  uint8_t cursor_at[3];
  context_callback_table_t callbacks;
};

static context_leds_t rgb_ptrs;
static chord_t chord;
static context_screen_t cs;

/* ---------------------------------------------------------------------- */

static inline void s_update_leds() {
  rgb_ptrs.magic_number = CONTEXT_LEDS_T;
  for (uint8_t i=0; i<3; i++) {
    rgb_ptrs.rgb_p[i] = note_color_rgb_i(chord.cursor_at[i]);
  }
}

/* ---------------------------------------------------------------------- */

static void s_line1_callback(context_t *c, void *data, v32_t v) {

}

static void s_display_callback(context_t *c, void *data, v32_t v) {
  static bitmap_t b;

  pcp_one_line_bitmap(&b);

  xSemaphoreTake(c->screen->mutex, portMAX_DELAY);

  for (int8_t i=-1; i<2; i++) {
    bitmap_clear(&b);
    for (uint8_t j=0; j<3; j++) {
      const char *n = note_color_note_name_i((chord.cursor_at[j]+12+i)%12);
      bitmap_draw_string(&b,
          j*CELL_WIDTH + CELL_WIDTH/2 - strlen(n)*TRIPLE_LINE_TEXT_FONT.Width/2,
          0, &TRIPLE_LINE_TEXT_FONT, n);
    }
    if (i == 0) bitmap_invert(&b);
    bitmap_copy_from(&c->screen->pane, &b, 0, (i+1)*TRIPLE_LINE_TEXT_FONT.Height);
  }

  xSemaphoreGive(c->screen->mutex);
}

static void s_re_callback(context_t *c, void *data, v32_t v) {
  uint8_t *cursor_at = (uint8_t *)data;

  *cursor_at += v.s;
  *cursor_at %= 12;

  s_update_leds();
}

static void button_forward_callback(context_t* c, void *data, v32_t value) {

}

/* ---------------------------------------------------------------------- */

void chord_init(context_t *c, context_t *parent) {

  /*
   *  Step 1 - Validate the unininitialized object and initialize any
   *           necessary values.
   */
  assert(chord.magic_number == UNINITIALIZED);
  chord.magic_number = CHORD_T;

  /*
   *  Step 2 - Define input device callbacks.
   */
  chord.callbacks.re[ROTARY_ENCODER_RED_OFFSET].callback=s_re_callback;
  chord.callbacks.re[ROTARY_ENCODER_RED_OFFSET].data=&chord.cursor_at[0];
  chord.callbacks.re[ROTARY_ENCODER_GREEN_OFFSET].callback=s_re_callback;
  chord.callbacks.re[ROTARY_ENCODER_GREEN_OFFSET].data=&chord.cursor_at[1];
  chord.callbacks.re[ROTARY_ENCODER_BLUE_OFFSET].callback=s_re_callback;
  chord.callbacks.re[ROTARY_ENCODER_BLUE_OFFSET].data=&chord.cursor_at[2];

  chord.callbacks.button[BUTTON_LOWER_OFFSET].callback=button_return_callback;

  /*
   *  Step 3 - Set up the labels on the screen
   */
  context_screen_init(&cs);
  context_screen_set_re_label(&cs, 0, "Note 1");
  context_screen_set_re_label(&cs, 1, "Note 2");
  context_screen_set_re_label(&cs, 2, "Note 3");
  cs.button_chars[0] = LAQUO;
  cs.button_chars[1] = 32;

  /*
   *  Step 4 - Set up the display callbacks
   */
  chord.callbacks.screen.callback=s_display_callback;
  chord.callbacks.screen.data=&chord;

  /*
   *  Step 5 - First update of the LEDs
   */
  s_update_leds();

  /*
   *  Step 6 - Initialize the underlying context
   */
  context_init(c, &chord.callbacks, c->screen, &rgb_ptrs, &chord);
}

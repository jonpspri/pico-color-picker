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

#include <string.h>
#include <stdio.h>

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* pico-color-piano includes */
#include "pcp.h"
#include "buttons.h"
#include "context.h"
#include "io_devices.h"
#include "rgb_encoders.h"

/*-----------------------------------------------------------*/

/*  I/O RGB Encoder States -- this may evolve to an "object" */

static uint32_t persistent_rgb;
static struct context_leds rgb_ptrs;

context_screen_t cs;

typedef struct {
  uint32_t magic_number;
  bool active;
  uint8_t value;
  uint8_t shift;
  uint8_t button_offset;
} rgb_encoder_t;

struct rgb_encoders_data {  /* typedef in rgb_encoders.h */
  uint32_t magic_number;
  SemaphoreHandle_t rgb_encoder_mutex;
  uint32_t *rgb;
  context_callback_table_t callbacks;
  rgb_encoder_t rgb_encoders[4];  /*  We waste storage to simplify lookup.  Maybe not necessary with callbacks?  */
}; /* rgb_encoders_data_t */


static uint32_t rgb_encoders_value(rgb_encoders_data_t *re) {
  uint32_t rgb=0u;
  xSemaphoreTake(re->rgb_encoder_mutex, portMAX_DELAY);
  for (int i=0; i<4; i++) {
    if (!re->rgb_encoders[i].active) continue;
    rgb |= re->rgb_encoders[i].value << re->rgb_encoders[i].shift;
  }
  xSemaphoreGive(re->rgb_encoder_mutex);
  return rgb;
}

static void rgb_encoders_ui_callback(context_t *c, void *data, v32_t v) {
  static char hex_color_value[8];

  rgb_encoders_data_t *re = ((rgb_encoders_data_t *) data);
  assert(re->magic_number == RGB_ENCODERS_DATA_T);

  persistent_rgb = rgb_encoders_value(re);
  sprintf(hex_color_value, "#%06lx", persistent_rgb);

  bitmap_clear(&c->screen->pane);
  bitmap_draw_string(&c->screen->pane, 0, 0, &SINGLE_LINE_TEXT_FONT, hex_color_value);

  xTaskNotifyIndexed(tasks.screen, NFCN_IDX_EVENT, (uint32_t)&cs, eSetValueWithOverwrite);
  xTaskNotifyIndexed(tasks.leds, NFCN_IDX_EVENT, 0, eSetValueWithOverwrite);
}

void rgb_encoders_re_callback(void *re_v, v32_t delta) {
  rgb_encoder_t *re = (rgb_encoder_t *)re_v;
  assert(re->magic_number = RGB_ENCODER_T);

  int16_t value = re->value;
  int8_t multiplier = button_depressed_p(re->button_offset) ? 0x1u : 0x11u;
  value += delta.s * multiplier;
  value = MAX(value, 0);
  value = MIN(value, 0xff);
  re->value = value;
};

bool rgb_encoders_context_init(context_t *context, context_t *parent, uint32_t *rgb) {
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

  rgb_encoders->callbacks.re_handlers[ROTARY_ENCODER_RED_OFFSET].callback=rgb_encoders_re_callback;
  rgb_encoders->callbacks.re_handlers[ROTARY_ENCODER_RED_OFFSET].data=&rgb_encoders->rgb_encoders[ROTARY_ENCODER_RED_OFFSET];
  rgb_encoders->callbacks.re_handlers[ROTARY_ENCODER_GREEN_OFFSET].callback=rgb_encoders_re_callback;
  rgb_encoders->callbacks.re_handlers[ROTARY_ENCODER_GREEN_OFFSET].data=&rgb_encoders->rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET];
  rgb_encoders->callbacks.re_handlers[ROTARY_ENCODER_BLUE_OFFSET].callback=rgb_encoders_re_callback;
  rgb_encoders->callbacks.re_handlers[ROTARY_ENCODER_BLUE_OFFSET].data=&rgb_encoders->rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET];

  button_register_generic_callbacks(&rgb_encoders->callbacks);

  rgb_encoders->callbacks.ui_update.callback=rgb_encoders_ui_callback;
  rgb_encoders->callbacks.ui_update.data=(void *)rgb_encoders;

  context_screen_init(&cs);
  context_screen_set_re_label(&cs, 0, "Red");
  context_screen_set_re_label(&cs, 1, "Green");
  context_screen_set_re_label(&cs, 2, "Blue");
  cs.button_chars[0] = LAQUO;
  cs.button_chars[1] = RAQUO;

  rgb_ptrs.magic_number = CONTEXT_LEDS_T;
  for (uint8_t i=0; i<WS2812_PIXEL_COUNT; i++)
    rgb_ptrs.rgb_p[i] = &persistent_rgb;

  return context_init(context, parent, &rgb_encoders->callbacks, &cs, rgb_encoders);
}

void rgb_encoders_enable(context_t *c) {
  rgb_encoders_data_t *re = (rgb_encoders_data_t *)c->context_data;
  assert(re->magic_number == RGB_ENCODERS_DATA_T);

  /* TEMPORARY - Tell the LEDs to display the single value here */
  xTaskNotifyIndexed(tasks.leds, NFCN_IDX_RGBS, (uint32_t)&rgb_ptrs, eSetValueWithOverwrite);
  context_enable(c);
}

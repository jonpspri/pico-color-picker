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
/*#include "bitmap.h"*/
#include "buttons.h"
#include "context.h"
#include "io_devices.h"
#include "rgb_encoders.h"

/*-----------------------------------------------------------*/

/*  I/O RGB Encoder States -- this may evolve to an "object" */

typedef struct rgb_encoder {
  bool active;
  uint8_t value;
  uint8_t shift;
  uint8_t button_offset;
} rgb_encoder_t;

#define RGB_ENCODERS_COUNT 4
struct rgb_encoders_data {
  SemaphoreHandle_t rgb_encoder_mutex;
  rgb_encoder_t rgb_encoders[4];  /*  We waste storage to simplify lookup.  Maybe not necessary with callbacks?  */
};

context_callback_table_t rgb_callbacks;
rgb_encoders_data_t rgb_encoders;

uint32_t rgb_encoders_value(rgb_encoders_data_t *re) {
  uint32_t rgb=0u;
  xSemaphoreTake(re->rgb_encoder_mutex, portMAX_DELAY);
  for (int i=0; i<RGB_ENCODERS_COUNT; i++) {
    if (!re->rgb_encoders[i].active) continue;
    rgb |= re->rgb_encoders[i].value << re->rgb_encoders[i].shift;
  }
  xSemaphoreGive(re->rgb_encoder_mutex);
  return rgb;
}

void rgb_encoders_ui_callback(void *data, v32_t v) {
  static context_screen_handle_t sc;
  static char hex_color_value[8];
  if (!sc) {
    sc = context_screen_init();
    context_screen_set_re_label(sc, 0, "Red");
    context_screen_set_re_label(sc, 1, "Green");
    context_screen_set_re_label(sc, 2, "Blue");
    context_screen_set_button_char(sc, 0, LAQUO);
    context_screen_set_button_char(sc, 1, RAQUO);
  }

  uint32_t rgb = rgb_encoders_value((rgb_encoders_data_t *)data);
  sprintf(hex_color_value, "#%06lx", rgb);

  Bitmap &bitmap=context_screen_bitmap(sc);
  bitmap.clear();
  bitmap.draw_string(0, 0, &SINGLE_LINE_TEXT_FONT, hex_color_value);

  xTaskNotifyIndexed(tasks.screen, 1, (uint32_t)sc, eSetValueWithOverwrite);
  xTaskNotifyIndexed(tasks.leds, 1, rgb, eSetValueWithOverwrite);
}

void rgb_encoders_re_callback(void *re_v, v32_t delta) {
  rgb_encoder_t *re = (rgb_encoder_t *)re_v;
  int16_t value = re->value;
  int8_t multiplier = button_depressed_p(re->button_offset) ? 0x1u : 0x11u;
  value += delta.s * multiplier;
  value = MAX(value, 0);
  value = MIN(value, 0xff);
  re->value = value;
};

void rgb_encoders_context_enable(context_handle_t context) {
  if(tasks.rotary_encoders)
    xTaskNotifyIndexed(tasks.rotary_encoders, 2, (uint32_t)&rgb_callbacks, eSetValueWithOverwrite);
  if(tasks.buttons)
    xTaskNotifyIndexed(tasks.buttons, 2, (uint32_t)&rgb_callbacks, eSetValueWithOverwrite);
}

context_handle_t rgb_encoders_context_init(uint32_t rgb) {
  rgb_encoders.rgb_encoder_mutex=xSemaphoreCreateMutex();

  rgb_encoders.rgb_encoders[ROTARY_ENCODER_RED_OFFSET].active = true;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_RED_OFFSET].shift = 16;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_RED_OFFSET].button_offset = BUTTON_RED_OFFSET;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_RED_OFFSET].value = rgb >> 16 & 0xff;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].active = true;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].shift = 8;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].button_offset = BUTTON_GREEN_OFFSET;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].value = rgb >> 8 & 0xff;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].active = true;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].shift = 0;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].button_offset = BUTTON_BLUE_OFFSET;
  rgb_encoders.rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].value = rgb & 0xff;

  rgb_callbacks.re_handlers[ROTARY_ENCODER_RED_OFFSET].callback=rgb_encoders_re_callback;
  rgb_callbacks.re_handlers[ROTARY_ENCODER_RED_OFFSET].data=&rgb_encoders.rgb_encoders[ROTARY_ENCODER_RED_OFFSET];
  rgb_callbacks.re_handlers[ROTARY_ENCODER_GREEN_OFFSET].callback=rgb_encoders_re_callback;
  rgb_callbacks.re_handlers[ROTARY_ENCODER_GREEN_OFFSET].data=&rgb_encoders.rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET];
  rgb_callbacks.re_handlers[ROTARY_ENCODER_BLUE_OFFSET].callback=rgb_encoders_re_callback;
  rgb_callbacks.re_handlers[ROTARY_ENCODER_BLUE_OFFSET].data=&rgb_encoders.rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET];

  button_register_generic_callbacks(&rgb_callbacks);

  rgb_callbacks.ui_update.callback=rgb_encoders_ui_callback;
  rgb_callbacks.ui_update.data=(void *)&rgb_encoders;

  return context_init(&rgb_callbacks, sizeof(rgb_encoders), &rgb_encoders);
}

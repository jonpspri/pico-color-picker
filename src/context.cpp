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

#include <stdlib.h>
#include <string.h>

#include "context.h"
#include "log.h"
#include "ssd1306.h"

extern "C" {
typedef struct context_screen {
  SemaphoreHandle_t mutex;
  Bitmap *pane;
  char re_labels[3][9];
  uint16_t button_chars[2];
} context_screen_t;
}

context_handle_t context_init(context_callback_table_t *callbacks, size_t context_data_size, void *context_data) {
  context_t* context = (context_t *)malloc(sizeof(context_t));
  context->context_data_size = context_data_size;
  context->context_data = context_data;
  return (context_handle_t)context;
}

static const uint8_t re_label_y_offset = SCREEN_HEIGHT - RE_LABEL_FONT.Height;
static const uint8_t re_label_total_width = SCREEN_WIDTH - BUTTON_LABEL_FONT.Width;

context_screen_handle_t context_screen_init() {
  context_screen_t *cs = new context_screen_t;
  // context_screen_t *cs = (context_screen_t *)memset(pvPortMalloc(sizeof(context_screen_t)),0,sizeof(context_screen_t));
  cs->mutex=xSemaphoreCreateMutex();
  cs->pane = new Bitmap(re_label_total_width, re_label_y_offset);
  memset(cs->re_labels,0,3*9);
  cs->button_chars[0]=0;
  cs->button_chars[1]=0;
  return (context_screen_handle_t)cs;
}

void context_screen_set_re_label(context_screen_handle_t csh, uint8_t i, const char *str) {
  context_screen_t *cs = (context_screen_t *)csh;
  strncat(cs->re_labels[i], str, 8);
}

void context_screen_set_button_char(context_screen_handle_t csh, uint8_t i, uint16_t c) {
  context_screen_t *cs = (context_screen_t *)csh;
  cs->button_chars[i] = c;
}

Bitmap &context_screen_bitmap(context_screen_handle_t csh) {
  context_screen_t *cs = (context_screen_t *)csh;
  return *cs->pane;
}

void context_screen_task(void *parm) {
  Bitmap *screen_buffer;
  ssd1306_t disp;
  context_screen_t *sc;

  /*  The screen needs a little time to warm up  */
  vTaskDelay(400 / portTICK_PERIOD_MS);

  log_info("%s", "Initializing Display...");
  disp.external_vcc=false;
  ssd1306_init(&disp, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_I2C_ADDRESS, SCREEN_I2C);
  ssd1306_clear(&disp);
  ssd1306_show(&disp);
  log_info("%s", "Display Initialized...");

  screen_buffer = new Bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);

  for( ;; ) {
    screen_buffer->clear();
    ssd1306_clear(&disp);

    if (!xTaskNotifyWaitIndexed( 1, 0u, 0xFFFFFFFFu, (uint32_t *)(&sc), portMAX_DELAY)) continue;

    xSemaphoreTake(sc->mutex, portMAX_DELAY);
    screen_buffer->copy_from(*sc->pane);
    screen_buffer->draw_string(0, re_label_y_offset, &TRIPLE_LINE_TEXT_FONT, sc->re_labels[0]);
    screen_buffer->draw_string(
        (re_label_total_width - TRIPLE_LINE_TEXT_FONT.Width*strnlen(sc->re_labels[1],8))/2,
        re_label_y_offset, &TRIPLE_LINE_TEXT_FONT, sc->re_labels[1]);
    screen_buffer->draw_string(
        re_label_total_width - TRIPLE_LINE_TEXT_FONT.Width*strnlen(sc->re_labels[2],8),
        re_label_y_offset, &TRIPLE_LINE_TEXT_FONT, sc->re_labels[2]);

    /*
     * Draw the chevrons if they're there
     */
    screen_buffer->draw_char(re_label_total_width, 0, &BUTTON_LABEL_FONT, sc->button_chars[0]);
    screen_buffer->draw_char(re_label_total_width,
        BUTTON_LABEL_FONT.Height, &BUTTON_LABEL_FONT, sc->button_chars[1]);
    xSemaphoreGive(sc->mutex);

    // TODO:  Make the ssd1306 Bitmap compatible.
    screen_buffer->rerender(SCREEN_WIDTH, SCREEN_HEIGHT, ssd1306_draw_pixel_callback, &disp);
    ssd1306_show(&disp);
  }
}

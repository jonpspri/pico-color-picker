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

#define RE_LABEL_Y_OFFSET (SCREEN_HEIGHT - RE_LABEL_FONT.Height)
#define RE_LABEL_TOTAL_WIDTH (SCREEN_WIDTH - BUTTON_LABEL_FONT.Width)

/* ---------------------------------------------------------------------- */

context_t *context_init(context_callback_table_t *callbacks, size_t context_data_size, void *context_data) {
  context_t* context = (context_t *)malloc(sizeof(context_t));
  context->context_data_size = context_data_size;
  context->context_data = context_data;
  return (context_t *)context;
}

/* ---------------------------------------------------------------------- */

context_screen_t *context_screen_init() {
  context_screen_t *cs = memset(pvPortMalloc(sizeof(context_screen_t)),0,sizeof(context_screen_t));
  cs->mutex=xSemaphoreCreateMutex();
  cs->pane = bitmap_init(RE_LABEL_TOTAL_WIDTH, RE_LABEL_Y_OFFSET);
  memset(cs->re_labels,0,3*9);
  cs->button_chars[0]=0;
  cs->button_chars[1]=0;
  return (context_screen_t *)cs;
}

void context_screen_set_re_label(context_screen_t *csh, uint8_t i, const char *str) {
  context_screen_t *cs = (context_screen_t *)csh;
  strncat(cs->re_labels[i], str, 8);
}

void context_screen_set_button_char(context_screen_t *csh, uint8_t i, uint16_t c) {
  context_screen_t *cs = (context_screen_t *)csh;
  cs->button_chars[i] = c;
}

bitmap_t *context_screen_bitmap(context_screen_t *csh) {
  context_screen_t *cs = (context_screen_t *)csh;
  return cs->pane;
}

/* ---------------------------------------------------------------------- */

void context_screen_task(void *parm) {
  // TODO - rework the ssd1306 code now that we understand it better
  bitmap_t *screen_buffer;
  ssd1306_t disp;
  context_screen_t *cs;

  /*  The screen needs a little time to warm up  */
  vTaskDelay(400 / portTICK_PERIOD_MS);

  log_info("%s", "Initializing Display...");
  disp.external_vcc=false;
  ssd1306_init(&disp, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_I2C_ADDRESS, SCREEN_I2C);
  ssd1306_clear(&disp);
  ssd1306_show(&disp);

  log_info("%s", "Display Initialized...");

  screen_buffer = bitmap_init(SCREEN_WIDTH, SCREEN_HEIGHT);

  for( ;; ) {
    bitmap_clear(screen_buffer);
    ssd1306_clear(&disp);

    if (!xTaskNotifyWaitIndexed( 1, 0u, 0xFFFFFFFFu, (uint32_t *)(& cs), portMAX_DELAY)) continue;

    xSemaphoreTake(cs->mutex, portMAX_DELAY);
    bitmap_copy_from(screen_buffer, cs->pane);
    bitmap_draw_string(screen_buffer, 0, RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, cs->re_labels[0]);
    bitmap_draw_string(screen_buffer,
        (RE_LABEL_TOTAL_WIDTH - TRIPLE_LINE_TEXT_FONT.Width*strnlen(cs->re_labels[1],8))/2,
        RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, cs->re_labels[1]);
    bitmap_draw_string(screen_buffer,
        RE_LABEL_TOTAL_WIDTH - TRIPLE_LINE_TEXT_FONT.Width*strnlen(cs->re_labels[2],8),
        RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, cs->re_labels[2]);

    /*
     * Draw the chevrons if they're there
     */
    bitmap_draw_char(screen_buffer, RE_LABEL_TOTAL_WIDTH, 0, &BUTTON_LABEL_FONT, cs->button_chars[0]);
    bitmap_draw_char(screen_buffer, RE_LABEL_TOTAL_WIDTH,
        BUTTON_LABEL_FONT.Height, &BUTTON_LABEL_FONT, cs->button_chars[1]);
    xSemaphoreGive(cs->mutex);

    // TODO:  Make the ssd1306 Bitmap compatible.
    bitmap_rerender(screen_buffer, SCREEN_WIDTH, SCREEN_HEIGHT, ssd1306_draw_pixel_callback, &disp);
    ssd1306_show(&disp);
  }
}

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

/** @file context.c
 *
 */
#include <stdlib.h>
#include <string.h>

#include "pcp.h"
#include "context.h"
#include "log.h"
#include "ssd1306.h"
#include "ws281x.h"

#define RE_LABEL_Y_OFFSET (SCREEN_HEIGHT - RE_LABEL_FONT.Height)
#define RE_LABEL_TOTAL_WIDTH (SCREEN_WIDTH - BUTTON_LABEL_FONT.Width)

/* ---------------------------------------------------------------------- */

bool context_init(context_t *context, context_t *parent, context_callback_table_t *callbacks, context_screen_t *screen, void *context_data) {
  if (context->context_data) return false;
  context->magic_number = CONTEXT_T;
  context->callbacks = callbacks;
  context->parent = parent;
  context->screen = screen;
  context->context_data = context_data;
  return true;
}

/* ---------------------------------------------------------------------- */

/** @brief Initialize a _context screen_ if necessary.  Can be safely called
 *         on the same object multiple times.
 *
 *  @param  cs The \ref context_screen_t object to be initialized.
 *
 *  @return `true` if futher initialization is required (e.g. set button text).
 */
bool context_screen_init(context_screen_t *cs) {
  if (cs->mutex) return false;
  cs->magic_number=CONTEXT_SCREEN_T;
  cs->mutex=xSemaphoreCreateMutex();
  bitmap_init(&cs->pane, RE_LABEL_TOTAL_WIDTH, RE_LABEL_Y_OFFSET, NULL);
  memset(cs->re_labels,0,3*9);
  cs->button_chars[0]=0;
  cs->button_chars[1]=0;
  return true;
}


/* ---------------------------------------------------------------------- */

void context_screen_task(void *parm) {
  static bitmap_t screen_buffer;
  static context_screen_t *cs;

  /*  The screen needs a little time to warm up  */
  vTaskDelay(400 / portTICK_PERIOD_MS);

  bitmap_init(&screen_buffer, SCREEN_WIDTH, SCREEN_HEIGHT, b_ssd1306_init);
  bitmap_clear(&screen_buffer);
  ssd1306_show((ssd1306_t *)screen_buffer.buffer);

  for( ;; ) {
    bitmap_clear(&screen_buffer);

    if (!xTaskNotifyWaitIndexed( 1, 0u, 0xFFFFFFFFu, (uint32_t *)(& cs), portMAX_DELAY)) continue;
    assert(cs->magic_number == CONTEXT_SCREEN_T);

    /* If an assert fails in the xSemaphoreTake, it likely means the ContextScreen is corrupt */
    xSemaphoreTake(cs->mutex, portMAX_DELAY);
    bitmap_copy_from(&screen_buffer, &cs->pane, 0, 0);
    bitmap_draw_string(&screen_buffer, 0, RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, cs->re_labels[0]);
    bitmap_draw_string(&screen_buffer,
        (RE_LABEL_TOTAL_WIDTH - TRIPLE_LINE_TEXT_FONT.Width*strnlen(cs->re_labels[1],8))/2,
        RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, cs->re_labels[1]);
    bitmap_draw_string(&screen_buffer,
        RE_LABEL_TOTAL_WIDTH - TRIPLE_LINE_TEXT_FONT.Width*strnlen(cs->re_labels[2],8),
        RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, cs->re_labels[2]);

    /*
     * Draw the chevrons if they're there
     */
    bitmap_draw_char(&screen_buffer, RE_LABEL_TOTAL_WIDTH, 0, &BUTTON_LABEL_FONT, cs->button_chars[0]);
    bitmap_draw_char(&screen_buffer, RE_LABEL_TOTAL_WIDTH,
        BUTTON_LABEL_FONT.Height, &BUTTON_LABEL_FONT, cs->button_chars[1]);
    xSemaphoreGive(cs->mutex);

    ssd1306_show((ssd1306_t *)screen_buffer.buffer);
  }
}

void context_leds_task(void * parm) {
  static struct context_leds *rgbs;
  for( ;; ) {
    /* Set the context_leds_object */
    xTaskNotifyWaitIndexed(NFCN_IDX_RGBS, 0u, 0u, (uint32_t *)&rgbs, rgbs ? 0 : portMAX_DELAY);
    if (!rgbs) continue;
    assert(rgbs->magic_number == CONTEXT_LEDS_T);

    ws2812_put_pixels(rgbs->rgb_p, 3);
    if (!xTaskNotifyWaitIndexed( 1, 0u, 0xFFFFFFFFu, NULL, portMAX_DELAY)) continue;
  }
}

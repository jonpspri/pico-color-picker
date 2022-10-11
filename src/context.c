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

/* ---------------------------------------------------------------------- */

bool context_init(context_t *context,
  context_t *parent,
  context_callback_table_t *callbacks,
  context_screen_t *screen,
  context_leds_t *leds,
  void *data
) {
  if (context->data) return false;
  context->magic_number = CONTEXT_T;
  context->callbacks = callbacks;
  context->parent = parent;
  context->screen = screen;
  context->leds = leds;
  context->data = data;
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

void context_display_task(void *parm) {
  static bitmap_t screen_buffer;
  static context_t *c;

  /*  The screen needs a little time to warm up  */
  vTaskDelay(400 / portTICK_PERIOD_MS);

  bitmap_init(&screen_buffer, SCREEN_WIDTH, SCREEN_HEIGHT, b_ssd1306_init);

  bitmap_clear(&screen_buffer);
  ssd1306_show((ssd1306_t *)screen_buffer.buffer);

  for( ;; ) {

    if (!xTaskNotifyWaitIndexed( NTFCN_IDX_EVENT, 0u, 0xFFFFFFFFu, (uint32_t *)(& c), portMAX_DELAY)) continue;
    assert(c->magic_number == CONTEXT_T);

    if (!(c->screen && c->callbacks->screen.callback)) continue;
    bitmap_clear(&c->screen->pane);
    if (c->callbacks->line1.callback) {
      c->callbacks->line1.callback(c, c->callbacks->line1.data, (v32_t)0ul);
    }
    c->callbacks->screen.callback(c, c->callbacks->screen.data, (v32_t)0ul);

    if (c->leds) ws2812_put_pixels(c->leds->rgb_p, 3);

    /* If an assert fails in the xSemaphoreTake, it likely means the ContextScreen is corrupt */
    bitmap_clear(&screen_buffer);
    xSemaphoreTake(c->screen->mutex, portMAX_DELAY);
    bitmap_copy_from(&screen_buffer, &c->screen->pane, 0, 0);
    bitmap_draw_string(&screen_buffer, 0, RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, c->screen->re_labels[0]);
    bitmap_draw_string(&screen_buffer,
        (RE_LABEL_TOTAL_WIDTH - TRIPLE_LINE_TEXT_FONT.Width*strnlen(c->screen->re_labels[1],8))/2,
        RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, c->screen->re_labels[1]);
    bitmap_draw_string(&screen_buffer,
        RE_LABEL_TOTAL_WIDTH - TRIPLE_LINE_TEXT_FONT.Width*strnlen(c->screen->re_labels[2],8),
        RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, c->screen->re_labels[2]);

    /*
     * Draw the chevrons if they're there
     */
    bitmap_draw_char(&screen_buffer, RE_LABEL_TOTAL_WIDTH, 0, &BUTTON_LABEL_FONT, c->screen->button_chars[0]);
    bitmap_draw_char(&screen_buffer, RE_LABEL_TOTAL_WIDTH,
        BUTTON_LABEL_FONT.Height, &BUTTON_LABEL_FONT, c->screen->button_chars[1]);
    xSemaphoreGive(c->screen->mutex);

    ssd1306_show((ssd1306_t *)screen_buffer.buffer);
  }
}

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

/** @file context.c */

#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"

#include "pcp.h"
#include "context.h"
#include "log.h"
#include "ssd1306.h"
#include "ws281x.h"

/* ---------------------------------------------------------------------- */

typedef struct context_frame {
  context_t *context;
  void *data;
} context_frame_t;

/* ---------------------------------------------------------------------- */

static QueueHandle_t context_stack;

/* ---------------------------------------------------------------------- */

static void s_context_enable(context_t *c) {
  log_trace("Enabling context %lx", (uint32_t)c);
  if(c->callbacks->enable.callback)
    c->callbacks->enable.callback(c, c->callbacks->enable.data, (v32_t)0ul);
  if (tasks.rotary_encoders)
    xTaskNotifyIndexed(tasks.rotary_encoders, NTFCN_IDX_CONTEXT, (uint32_t)c, eSetValueWithOverwrite);
  if (tasks.buttons)
    xTaskNotifyIndexed(tasks.buttons, NTFCN_IDX_CONTEXT, (uint32_t)c, eSetValueWithOverwrite);
  context_notify_display_task(c);
}

/* ---------------------------------------------------------------------- */

static void context_free(void *v) {
  context_t *c = (context_t *)v;
  ASSERT_IS_A(c, CONTEXT_T);
  pcp_free(c->data);
  vPortFree(c);
}

bool context_init(context_t *context,
  context_callback_table_t *callbacks,
  context_screen_t *screen,
  void *data
) {
  if (context->data) return false;
  context->pcp.magic_number = CONTEXT_T | FREEABLE_P;
  context->pcp.free = context_free;
  context->callbacks = callbacks;
  context->screen = screen;
  context->config_q = xQueueCreate(1, sizeof(context_config_msg_t));
  context->data = data;
  return true;
}

context_t *context_alloc( context_callback_table_t *callbacks,
  context_screen_t *screen,
  void *data
) {
  context_t *context = pcp_zero_malloc(sizeof(context_t));
  context_init(context, callbacks, screen, data);
  return context;
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
  static context_leds_t *leds;

  /*  The screen needs a little time to warm up  */
  vTaskDelay(400 / portTICK_PERIOD_MS);

  bitmap_init(&screen_buffer, SCREEN_WIDTH, SCREEN_HEIGHT, b_ssd1306_init);

  bitmap_clear(&screen_buffer);
  ssd1306_show((ssd1306_t *)screen_buffer.buffer);

  for( ;; ) {

    while (!xTaskNotifyWaitIndexed( NTFCN_IDX_EVENT, 0u, 0xFFFFFFFFu, (uint32_t *)(& c), portMAX_DELAY));

    xTaskNotifyWaitIndexed( NTFCN_IDX_LEDS, 0u, 0u, (uint32_t *)(&leds), 0u);

    ASSERT_IS_A(c, CONTEXT_T);

    if (!(c->screen && c->callbacks->screen.callback)) continue;
    bitmap_clear(&c->screen->pane);
    c->callbacks->screen.callback(c, c->callbacks->screen.data, (v32_t)0ul);

    if (leds) ws2812_put_pixels(leds->rgb_p, 3);

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

void context_push(context_t *c, void *frame_data) {
  if (!context_stack) context_stack = xQueueCreate(10, sizeof(context_frame_t));
  log_trace("Pushing context %lx", c);
  ASSERT_IS_A(c, CONTEXT_T);  /*  Make sure the context is (somewhat) initialized  */
  context_frame_t f = { c, frame_data };
  BaseType_t success = xQueueSendToFront(context_stack, &f, 0);
  assert(success == pdTRUE);
  s_context_enable(c);
}

context_t *context_current() {
  context_frame_t f;
  BaseType_t success = xQueuePeek(context_stack, &f, 0);
  assert(success == pdTRUE);
  return f.context;
}

void *context_frame_data() {
  context_frame_t f;
  BaseType_t success = xQueuePeek(context_stack, &f, 0);
  assert(success == pdTRUE);
  return f.data;
}

context_t *context_pop() {
  context_frame_t f;
  BaseType_t success = xQueueReceive(context_stack, &f, 0);
  assert(success == pdTRUE);
  s_context_enable(context_current());
  return f.context;
}

uint32_t context_stack_depth() {
  return uxQueueMessagesWaiting(context_stack);
}

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

static void s_context_enable(context_t *c)
{
    log_trace("Enabling context %lx", (uint32_t) c);
    if (c->enable_ccb.callback) {
        c->enable_ccb.callback(c, c->enable_ccb.data,
                (v32_t) 0ul
                );
    }
    if (tasks.rotary_encoders) {
        xTaskNotifyIndexed(tasks.rotary_encoders, NTFCN_IDX_CONTEXT,
                (uint32_t) c, eSetValueWithOverwrite
                );
    }
    if (tasks.buttons) {
        xTaskNotifyIndexed(tasks.buttons, NTFCN_IDX_CONTEXT, (uint32_t) c,
                eSetValueWithOverwrite
                );
    }
    context_notify_display_task(c);
} /* s_context_enable */

void context_set_button_char(context_t *c, uint8_t offset, int16_t v)
{
    c->button_chars[offset] = v;
}

bitmap_t *context_get_drawing_pane(context_t *c)
{
    return ( c ?: context_current() )->pane;
}

context_callback_t *context_get_button_callback(context_t *c, uint8_t i)
{
    assert(i < IO_PIO_SLOTS);
    return &c->button_ccb[i];
}

context_callback_t *context_get_display_callback(context_t *c)
{
    return &c->display_ccb;
}

/* ---------------------------------------------------------------------- */

static void context_free(void *v)
{
    context_t *c = (context_t *) v;
    ASSERT_IS_A(c, CONTEXT_T);
    pcp_free(c->pane);
    pcp_free(c->data);
    vPortFree(c);
}

void context_builder_init()
{
    assert( !pvTaskGetThreadLocalStoragePointer(NULL, ThLS_BLDR_CTX) );
    context_t *context = pcp_zero_malloc( sizeof( context_t ) );
    context->pcp.magic_number = CONTEXT_T;
    context->pcp.free_f = context_free;
    for (uint8_t i = 0; i<IO_PIO_SLOTS; i++) {context->button_chars[i] = 32;}
    context->pane = bitmap_alloc(RE_LABEL_TOTAL_WIDTH, RE_LABEL_Y_OFFSET, NULL);
    vTaskSetThreadLocalStoragePointer(NULL, ThLS_BLDR_CTX, context);
}

void context_builder_set_re(uint8_t re_offset, uint8_t button_offset,
        context_callback_f re_callback, void *re_data, context_callback_f
        button_callback, void *button_data, const char *label)
{
    context_t *c = (context_t *) pvTaskGetThreadLocalStoragePointer(NULL,
            ThLS_BLDR_CTX
            );
    ASSERT_IS_A(c, CONTEXT_T);
    c->re_ccb[re_offset].callback = re_callback;
    c->re_ccb[re_offset].data = re_data;
    c->button_ccb[button_offset].callback = button_callback;
    c->button_ccb[button_offset].data = button_data;
    strncat(c->re_labels[re_offset], label, RE_LABEL_LEN);
} /* context_builder_set_re */

void context_builder_set_button(uint8_t offset, context_callback_f callback,
        void *data, int16_t label)
{
    context_t *c = (context_t *) pvTaskGetThreadLocalStoragePointer(NULL,
            ThLS_BLDR_CTX
            );
    ASSERT_IS_A(c, CONTEXT_T);
    c->button_ccb[offset].callback = callback;
    c->button_ccb[offset].data = data;
    c->button_chars[offset] = label;
}

void context_builder_set_enable_callback(context_callback_f callback,
        void *data)
{
    context_t *c = (context_t *) pvTaskGetThreadLocalStoragePointer(NULL,
            ThLS_BLDR_CTX
            );
    ASSERT_IS_A(c, CONTEXT_T);
    c->enable_ccb.callback = callback;
    c->enable_ccb.data = data;
}

void context_builder_set_display_callback(context_callback_f callback,
        void *data)
{
    context_t *c = (context_t *) pvTaskGetThreadLocalStoragePointer(NULL,
            ThLS_BLDR_CTX
            );
    ASSERT_IS_A(c, CONTEXT_T);
    c->display_ccb.callback = callback;
    c->display_ccb.data = data;
}

void context_builder_set_data(void *data)
{
    context_t *c = (context_t *) pvTaskGetThreadLocalStoragePointer(NULL,
            ThLS_BLDR_CTX
            );
    ASSERT_IS_A(c, CONTEXT_T);
    c->data = data;
}

context_t *context_builder_finalize()
{
    context_t *c = (context_t *) pvTaskGetThreadLocalStoragePointer(NULL,
            ThLS_BLDR_CTX
            );
    ASSERT_IS_A(c, CONTEXT_T);
    vTaskSetThreadLocalStoragePointer(NULL, ThLS_BLDR_CTX, NULL);
    return c;
}

/* ---------------------------------------------------------------------- */

/** @brief Initialize a _context screen_ if necessary.  Can be safely called
 *         on the same object multiple times.
 *
 *  @param  cs The \ref context_screen_t object to be initialized.
 *
 *  @return `true` if futher initialization is required (e.g. set button text).
 */

/* ---------------------------------------------------------------------- */

void context_display_task(void *parm)
{
    static bitmap_t *screen_buffer;
    static context_t *c;
    static context_leds_t *leds;

    /*  The screen needs a little time to warm up  */
    vTaskDelay(400 / portTICK_PERIOD_MS);

    if (!screen_buffer) {
        screen_buffer = bitmap_alloc(SCREEN_WIDTH, SCREEN_HEIGHT,
                b_ssd1306_init
                );
    }

    bitmap_clear(screen_buffer);
    ssd1306_show( (ssd1306_t *) screen_buffer->buffer );

    for ( ;;) {
        while ( !xTaskNotifyWaitIndexed(NTFCN_IDX_EVENT, 0u, 0xFFFFFFFFu,
                (uint32_t *) ( &c ), portMAX_DELAY
                ) ) {;}

        xTaskNotifyWaitIndexed(NTFCN_IDX_LEDS, 0u, 0u, (uint32_t *) ( &leds ),
                0u
                );

        ASSERT_IS_A(c, CONTEXT_T);

        if ( !( c->display_ccb.callback ) ) {
            continue;
        }
        bitmap_clear(c->pane);
        c->display_ccb.callback(c, c->display_ccb.data, (v32_t) 0ul);

        if (leds) {
            ws2812_put_pixels(leds->rgb_p, 3);
        }

        /* If an assert fails in the xSemaphoreTake, it likely means the ContextScreen is corrupt */
        bitmap_clear(screen_buffer);

        bitmap_copy_from(screen_buffer, c->pane, 0, 0);
        bitmap_draw_string(screen_buffer, 0, RE_LABEL_Y_OFFSET,
                &TRIPLE_LINE_TEXT_FONT, c->re_labels[re_offsets[0]]
                );
        bitmap_draw_string(screen_buffer,
                ( RE_LABEL_TOTAL_WIDTH - TRIPLE_LINE_TEXT_FONT.Width *
                  strnlen(c->re_labels[re_offsets[2]],
                          8
                          ) ) / 2,
                RE_LABEL_Y_OFFSET,
                &TRIPLE_LINE_TEXT_FONT,
                c->re_labels[re_offsets[1]]
                );
        bitmap_draw_string(screen_buffer,
                RE_LABEL_TOTAL_WIDTH - TRIPLE_LINE_TEXT_FONT.Width *
                strnlen(c->re_labels[re_offsets[2]],8),
                RE_LABEL_Y_OFFSET, &TRIPLE_LINE_TEXT_FONT, c->re_labels[re_offsets[2]]
                );

        /*
         * Draw the chevrons if they're there
         */
        bitmap_draw_char(screen_buffer, RE_LABEL_TOTAL_WIDTH, 0,
                &BUTTON_LABEL_FONT, c->button_chars[0]
                );
        bitmap_draw_char(screen_buffer, RE_LABEL_TOTAL_WIDTH,
                BUTTON_LABEL_FONT.Height, &BUTTON_LABEL_FONT, c->button_chars[1]
                );

        ssd1306_show( (ssd1306_t *) screen_buffer->buffer );
    }
} /* context_display_task */

void context_push(context_t *c, void *frame_data)
{
    if (!context_stack) {
        context_stack = xQueueCreate(10,
                sizeof( context_frame_t )
                );
    }
    log_trace("Pushing context %lx", c);
    ASSERT_IS_A(c, CONTEXT_T); /*  Make sure the context is (somewhat) initialized  */
    context_frame_t f = { c, frame_data };
    BaseType_t success = xQueueSendToFront(context_stack, &f, 0);
    assert(success == pdTRUE);
    s_context_enable(c);
} /* context_push */

context_t *context_current()
{
    context_frame_t f;
    BaseType_t success = xQueuePeek(context_stack, &f, 0);
    assert(success == pdTRUE);
    return f.context;
}

void *context_frame_data()
{
    context_frame_t f;
    BaseType_t success = xQueuePeek(context_stack, &f, 0);
    assert(success == pdTRUE);
    return f.data;
}

context_t *context_pop()
{
    context_frame_t f;
    BaseType_t success = xQueueReceive(context_stack, &f, 0);
    assert(success == pdTRUE);
    s_context_enable( context_current() );
    return f.context;
}

uint32_t context_stack_depth()
{
    return uxQueueMessagesWaiting(context_stack);
}

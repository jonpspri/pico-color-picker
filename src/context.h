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

#ifndef __CONTEXT_H
#define __CONTEXT_H

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "pico/stdlib.h"

#include "pcp.h"
#include "bitmap.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file context.h
 *
 * CONTEXTS:
 *
 * The basic idea of a "Context" is to contain the I/O and screen handling
 * for a single user interaction.  Such interactions could be:
 *
 * - Main menu
 * - Setting
 * - Color picker list
 * - Individual color picker
 *
 * To that end, we'll need to keep a "context tree" list that allows for moving
 * up and down a context tree.  Not sure how that's all going to work out
 * quite yet, but programming is an adventure.
 *
 * We'll start by moving the implementation of the the color dial picker to
 * be a context.
 */

/* Forward declarations */

typedef struct {
    context_callback_f callback;
    void *data;
} context_callback_t;

/** @brief Hold the current configuration of the three WS2812 RGBs on the board.
 *
 *  Note that we use pointers so that if a different portion of the program
 *  changes the actual value, the display will automatically update.  Think of
 *  this as a quick and dirty "observer" pattern.
 */
typedef struct context_leds {
    uint32_t magic_number;
    uint32_t *rgb_p[WS2812_PIXEL_COUNT];
} context_leds_t;

#define CTX_MSG_TYP_LINE1_CB 0x01

typedef struct context_config_msg {
    uint32_t msg_type;
    void *msg_data;
} context_config_msg_t;

#define RE_LABEL_LEN 8
struct context {
    pcp_t pcp;
    QueueHandle_t config_q;
    bitmap_t *pane;

    context_callback_t re_ccb[IO_PIO_SLOTS/2];
    char re_labels[IO_PIO_SLOTS/2][RE_LABEL_LEN + 1];
    context_callback_t button_ccb[IO_PIO_SLOTS];
    uint16_t button_chars[IO_PIO_SLOTS];

    context_callback_t enable_ccb;
    context_callback_t display_ccb;

    void *data;
};

typedef struct task_list {
    TaskHandle_t rotary_encoders;
    TaskHandle_t buttons;
    TaskHandle_t display;
} task_list_t;

extern task_list_t tasks;

/* ---------------------------------------------------------------------- */

void context_display_task(void *parm);

void context_push(context_t *c, void *f);
context_t *context_current();
void *context_frame_data();
context_t *context_pop();
uint32_t context_stack_depth();

static inline void context_notify_display_task(context_t *c)
{
    xTaskNotifyIndexed(tasks.display, NTFCN_IDX_EVENT, (uint32_t) c,
            eSetValueWithOverwrite
            );
}

void context_set_button_char(context_t *c, uint8_t offset, int16_t v);
static inline void context_set_upper_button_char(context_t *c, int16_t v)
{
    context_set_button_char(c, BUTTON_UPPER_OFFSET, v);
}
static inline void context_set_lower_button_char(context_t *c, int16_t v)
{
    context_set_button_char(c, BUTTON_LOWER_OFFSET, v);
}

bitmap_t *context_get_drawing_pane(context_t *);
context_callback_t *context_get_button_callback(context_t *, uint8_t);
context_callback_t *context_get_display_callback(context_t *);

/* ---------------------------------------------------------------------- */

void context_builder_init();

void context_builder_set_re(uint8_t re_offset, uint8_t button_offset,
        context_callback_f re_callback, void *re_data, context_callback_f
        button_callback, void *button_data, const char *label);
static inline void context_builder_set_red_re(
        context_callback_f re_callback, void *re_data, context_callback_f
        button_callback, void *button_data, const char *label)
{
    context_builder_set_re(RE_RED_OFFSET, BUTTON_RED_OFFSET, re_callback,
            re_data, button_callback, button_data, label
            );
}
static inline void context_builder_set_green_re(
        context_callback_f re_callback, void *re_data, context_callback_f
        button_callback, void *button_data, const char *label)
{
    context_builder_set_re(RE_GREEN_OFFSET, BUTTON_GREEN_OFFSET, re_callback,
            re_data, button_callback, button_data, label
            );
}
static inline void context_builder_set_blue_re(
        context_callback_f re_callback, void *re_data, context_callback_f
        button_callback, void *button_data, const char *label)
{
    context_builder_set_re(RE_BLUE_OFFSET, BUTTON_BLUE_OFFSET, re_callback,
            re_data, button_callback, button_data, label
            );
}

void context_builder_set_button(uint8_t offset, context_callback_f callback,
        void *data, int16_t label);
static inline void context_builder_set_upper_button(context_callback_f callback,
        void *data, int16_t label)
{
    context_builder_set_button(BUTTON_UPPER_OFFSET, callback, data, label);
}
static inline void context_builder_set_lower_button(context_callback_f callback,
        void *data, int16_t label)
{
    context_builder_set_button(BUTTON_LOWER_OFFSET, callback, data, label);
}

void context_builder_set_display_callback(context_callback_f callback,
        void *data);
void context_builder_set_enable_callback(context_callback_f callback,
        void *data);

void context_builder_set_data(void *data);

context_t *context_builder_finalize();

#ifdef __cplusplus
}
#endif

#endif

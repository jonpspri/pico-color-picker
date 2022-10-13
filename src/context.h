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
typedef struct context context_t;

typedef union v32 {
#ifdef __cplusplus
  v32(int32_t x) : s(x) { };
  v32(uint32_t x) : u(x) { };
#endif
  int32_t s;
  uint32_t u;
} v32_t;

typedef void (*context_callback_f)(context_t *c, void *, v32_t);
typedef struct {
  context_callback_f callback;
  void *data;
} context_callback_t;

typedef struct {
  context_callback_t enable;
  context_callback_t button[IO_PIO_SLOTS];
  context_callback_t re[IO_PIO_SLOTS/2];
  context_callback_t screen;
} context_callback_table_t;

typedef struct context_screen {
  uint32_t magic_number;
  SemaphoreHandle_t mutex;
  char re_labels[ROTARY_ENCODER_COUNT][9];
  uint16_t button_chars[BUTTON_COUNT];
  bitmap_t pane;
} context_screen_t;

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

struct context {
  pcp_t pcp;
  context_callback_table_t *callbacks;
  context_screen_t *screen;
  QueueHandle_t config_q;
  void *data;
};

typedef struct task_list {
  TaskHandle_t rotary_encoders;
  TaskHandle_t buttons;
  TaskHandle_t display;
} task_list_t;

extern struct context_rgbs ws2812_rgbs;

extern task_list_t tasks;

bool context_init(context_t *context,
    context_callback_table_t *callbacks,
    context_screen_t *screen,
    void *context_data);
context_t *context_alloc(
    context_callback_table_t *callbacks,
    context_screen_t *screen,
    void *context_data);

bool context_screen_init(context_screen_t *cs);

void context_display_task(void *parm);

void context_push(context_t *c, void *f);
context_t *context_current();
void *context_frame_data();
context_t *context_pop();
uint32_t context_stack_depth();

static inline void context_screen_set_re_label(context_screen_t *csh, uint8_t i, const char *label) {
  strncpy(csh->re_labels[i], label, 9);
}

static inline void context_notify_display_task(context_t *c) {
  xTaskNotifyIndexed(tasks.display, NTFCN_IDX_EVENT, (uint32_t)c, eSetValueWithOverwrite);
}

static inline void context_screen_set_button_char(context_screen_t *cs, uint8_t i, uint16_t c) { cs->button_chars[i] = c; }

#ifdef __cplusplus
}
#endif

#endif

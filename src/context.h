/* vim: set ft=cpp:
 *
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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "pico/stdlib.h"

#include "bitmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------------
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

union v32 {
#ifdef __cplusplus
  v32(int32_t x) : s(x) { };
  v32(uint32_t x) : u(x) { };
#endif
  int32_t s;
  uint32_t u;
};

typedef union v32 v32_t;

typedef struct context_callback {
  void (*callback)(void *, v32_t);
  void *data;
} context_callback_t;

typedef struct context_callback_table {
  context_callback_t button_handlers[8];
  context_callback_t re_handlers[4];
  context_callback_t ui_update;
} context_callback_table_t;

typedef struct context {
  context_callback_table_t *callbacks;
  size_t context_data_size;
  void *context_data;
} context_t;
typedef void *context_handle_t;

typedef void *context_screen_handle_t;

typedef struct task_list {
  TaskHandle_t rotary_encoders;
  TaskHandle_t buttons;
  TaskHandle_t screen;
  TaskHandle_t leds;
} task_list_t;

extern task_list_t tasks;

extern context_handle_t context_init(context_callback_table_t *callbacks, size_t context_data_size, void *context_data);
extern context_screen_handle_t context_screen_init();
extern void context_screen_set_re_label(context_screen_handle_t, uint8_t, const char *);
extern void context_screen_set_button_char(context_screen_handle_t, uint8_t, uint16_t);

extern void context_screen_task(void *parm);
#ifdef __cplusplus
}
#endif

extern Bitmap &context_screen_bitmap(context_screen_handle_t);

#endif

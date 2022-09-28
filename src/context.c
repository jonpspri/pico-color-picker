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

#include "FreeRTOS.h"
#include "log.h"

#include <string.h>
#include "pico/stdlib.h"

#ifndef IO_HANDLER_COUNT
#define IO_HANDLER_COUNT 4
#endif

#ifndef MAX_CONTEXT_COUNT
#define MAX_CONTEXT_COUNT 4
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

typedef struct context context_t;
typedef void (*screen_handler_f)(void);
typedef void (*io_handler_f)(context_t *context);

struct context {
  io_handler_f io_handlers[IO_HANDLER_COUNT];
};

static context_t contexts[MAX_CONTEXT_COUNT];
static int context_count;

context_t *create_context(io_handler_f *io_handlers) {
  if (context_count >= MAX_CONTEXT_COUNT) panic("Context count overflow.");
  context_t *c = &contexts[context_count++];
  memcpy(c->io_handlers, io_handlers, sizeof(io_handler_f)*IO_HANDLER_COUNT);
  return c;
}

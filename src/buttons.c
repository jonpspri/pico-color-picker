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

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* pico-color-picker includes */
#include "context.h"
#include "io_devices.h"

/*-----------------------------------------------------------*/

/*  So far, we're just tracking the state of the buttons */

uint8_t button_depressed;

void button_task(void *parm) {
  context_t *context = NULL;
  uint32_t bits = 0u;

  io_devices_register_button(BUTTON_UPPER_OFFSET);
  io_devices_register_button(BUTTON_LOWER_OFFSET);
  io_devices_register_button(BUTTON_RED_OFFSET);
  io_devices_register_button(BUTTON_GREEN_OFFSET);
  io_devices_register_button(BUTTON_BLUE_OFFSET);
  io_devices_init_buttons(BUTTON_LOW_PIN, BUTTON_SM);

  for( ;; ) {
    /* Update the callbacks list if necessary */
    xTaskNotifyWaitIndexed(NFCN_IDX_CONTEXT, 0u, 0u, (uint32_t *)&context, 0);
    assert(!context || context->magic_number == CONTEXT_T);

    for (int i=0; context && i<8; i++, bits >>= 2) {
      if(!(bits & 1u)) continue;
      if (bits & 2u) {
        button_depressed |= 1<<i;
      } else {
        button_depressed &= ~(1<<i);
      }
      context_callback_t *c = &context->callbacks->button_handlers[i];
      if(c->callback) c->callback(c->data, (v32_t)(bits & 2u));
    }

    if (context->callbacks->ui_update.callback) {
      context->callbacks->ui_update.callback(context, context->callbacks->ui_update.data, (v32_t)0ul);
    }

    if (!xTaskNotifyWaitIndexed(1, 0u, 0xFFFFFFFFu, &bits, portMAX_DELAY)) continue;
  }
}

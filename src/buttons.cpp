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

static bool button_depressed[8];
bool button_depressed_p(uint8_t index) { return button_depressed[index]; }

static void process_button(void *button, v32_t new_state) {
  *((bool *)button) = new_state.u?true:false;
};

void button_register_generic_callbacks(context_callback_table_t *callbacks) {
  /*
   * TODO - Should the io_devices registration really be here?
   */
  io_devices_register_button(BUTTON_UPPER_OFFSET);
  callbacks->button_handlers[BUTTON_UPPER_OFFSET].callback=process_button;
  callbacks->button_handlers[BUTTON_UPPER_OFFSET].data=&button_depressed[BUTTON_UPPER_OFFSET];

  io_devices_register_button(BUTTON_LOWER_OFFSET);
  callbacks->button_handlers[BUTTON_LOWER_OFFSET].callback=process_button;
  callbacks->button_handlers[BUTTON_LOWER_OFFSET].data=&button_depressed[BUTTON_LOWER_OFFSET];

  io_devices_register_button(BUTTON_RED_OFFSET);
  callbacks->button_handlers[BUTTON_RED_OFFSET].callback=process_button;
  callbacks->button_handlers[BUTTON_RED_OFFSET].data=&button_depressed[BUTTON_RED_OFFSET];

  io_devices_register_button(BUTTON_GREEN_OFFSET);
  callbacks->button_handlers[BUTTON_GREEN_OFFSET].callback=process_button;
  callbacks->button_handlers[BUTTON_GREEN_OFFSET].data=&button_depressed[BUTTON_GREEN_OFFSET];

  io_devices_register_button(BUTTON_BLUE_OFFSET);
  callbacks->button_handlers[BUTTON_BLUE_OFFSET].callback=process_button;
  callbacks->button_handlers[BUTTON_BLUE_OFFSET].data=&button_depressed[BUTTON_BLUE_OFFSET];
}

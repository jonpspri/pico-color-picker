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

#include <stdio.h>
#include <string.h>
#if LIB_PICO_STDIO_USB && DEBUG
#include <tusb.h>
#endif

/* FreeRTOS Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Pico SDK Includes */
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

/* pico-color-picker includes */
#include "context.h"
#include "colors.h"
#include "io_devices.h"
#include "log.h"
#include "rgb_encoders.h"
#include "ws281x.h"

#include "fonts/font.h"

/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) xTask;

    /* Run time stack overflow checking is performed if
    configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.  pxCurrentTCB can be
    inspected in the debugger if the task name passed into this function is
    corrupt. */
    for( ;; );
}

/*-----------------------------------------------------------*/

/*
 *  Task definitions
 */

task_list_t tasks;

void task_rotary_encoders(void *parm) {
  io_devices_register_encoder(ROTARY_ENCODER_RED_OFFSET, ROTARY_ENCODER_RED_INVERTED);
  io_devices_register_encoder(ROTARY_ENCODER_GREEN_OFFSET, ROTARY_ENCODER_GREEN_INVERTED);
  io_devices_register_encoder(ROTARY_ENCODER_BLUE_OFFSET, ROTARY_ENCODER_BLUE_INVERTED);
  io_devices_init_encoders(ROTARY_ENCODER_LOW_PIN, ROTARY_ENCODER_SM);

  uint32_t bits = 0u;
  context_callback_table_t *callbacks = NULL;

  for( ;; ) {
    /* Update the callbacks list if necessary */
    xTaskNotifyWaitIndexed(2, 0u, 0u, (uint32_t *)(&callbacks), 0);

    for (int i=0; callbacks && i<4; i++, bits >>= 2) {
      context_callback_t *c = &callbacks->re_handlers[i];
      if(!c->callback || !(bits & 3u)) continue;
      v32_t delta = (v32_t)((int32_t)0);
      if(bits & 1u) delta.s=1;
      if(bits & 2u) delta.s=-1;
      c->callback(c->data, delta);
    }

    if (callbacks->ui_update.callback) callbacks->ui_update.callback(callbacks->ui_update.data, (v32_t)0ul);

    if (!xTaskNotifyWaitIndexed(1, 0u, 0xFFFFFFFFu, &bits, portMAX_DELAY)) continue;
  }
}

void task_buttons(void *parm) {
  io_devices_register_button(BUTTON_UPPER_OFFSET);
  io_devices_register_button(BUTTON_LOWER_OFFSET);
  io_devices_register_button(BUTTON_RED_OFFSET);
  io_devices_register_button(BUTTON_GREEN_OFFSET);
  io_devices_register_button(BUTTON_BLUE_OFFSET);
  io_devices_init_buttons(BUTTON_LOW_PIN, BUTTON_SM);

  uint32_t bits = 0u;
  context_callback_table_t *callbacks = NULL;
  for( ;; ) {
    /* Update the callbacks list if necessary */
    xTaskNotifyWaitIndexed(2, 0u, 0u, (uint32_t *)(&callbacks), 0);

    for (int i=0; callbacks && i<8; i++, bits >>= 2) {
      context_callback_t *c = &callbacks->button_handlers[i];
      if(!c->callback || !(bits & 1u)) continue;
      c->callback(c->data, (v32_t)(bits & 2u));
    }

    if (callbacks->ui_update.callback) callbacks->ui_update.callback(callbacks->ui_update.data, (v32_t)0ul);

    if (!xTaskNotifyWaitIndexed(1, 0u, 0xFFFFFFFFu, &bits, portMAX_DELAY)) continue;
  }
}

void task_leds(void * parm) {
  uint32_t rgb;
  for( ;; ) {
    if (!xTaskNotifyWaitIndexed( 1, 0u, 0xFFFFFFFFu, &rgb, portMAX_DELAY)) continue;
    ws281x_sparkle_pixels(1, &rgb);
  }
}


int main() {

  stdio_init_all();
#if LIB_PICO_STDIO_USB && !defined(NDEBUG)
  while (!stdio_usb_connected()) { sleep_ms(100); }
#endif
#ifdef LOG_LEVEL
  log_set_level(LOG_LEVEL);
#endif
  log_info("%s", "Initializing PIO...");
  ws281x_pio_init();

  log_info("%s", "Initializing I2C...");
  i2c_init(SCREEN_I2C, 400000);
  gpio_set_function(SCREEN_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCREEN_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(SCREEN_SDA_PIN);
  gpio_pull_up(SCREEN_SCL_PIN);

  /*
   * Start the tasks
   */
  xTaskCreate(task_rotary_encoders, "Rotary Encoders Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &tasks.rotary_encoders);

  xTaskCreate(task_buttons, "Buttons Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &tasks.buttons);

  /*
   * The current approach to using FreeType2 under the covers here leads to a
   * large demand for stack.  Some optimization may be in order, but not yet...
   */
  xTaskCreate(context_screen_task, "Screen Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &tasks.screen);

  xTaskCreate(task_leds, "LEDs Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &tasks.leds);

  /* rgb_encoders_context_enable( rgb_encoders_context_init(0) ); */
  colors_context_enable( colors_context_init() );

  vTaskStartScheduler();

  panic("This should not be reached.");
}

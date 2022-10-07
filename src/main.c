/*
 * SPDX-FileCopyrightText: 2022 Jonathan Springer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
#include "button.h"
#include "context.h"
#include "color.h"
#include "input.h"
#include "log.h"
#include "rgb_encoder.h"
#include "rotary_encoder.h"
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
    panic("Stack Overflow.");
    /* for ( ;; ); */
}

/*-----------------------------------------------------------*/

void log_lock(bool acq, void *data) {
  if (acq) xSemaphoreTake((SemaphoreHandle_t)data, portMAX_DELAY);
  else xSemaphoreGive((SemaphoreHandle_t)data);
}

task_list_t tasks;
int main() {
  static context_t rgb_context;  /* TEMPORARY */
  static uint32_t rgb = 0;       /* TEMPORARY */
  static context_t menu_context; /* TEMPORARY */

  stdio_init_all();
#if LIB_PICO_STDIO_USB && !defined(NDEBUG)
  while (!stdio_usb_connected()) { sleep_ms(100); }
#endif
  log_set_lock(log_lock, (void *)xSemaphoreCreateMutex());
#ifdef LOG_LEVEL
  log_set_level(LOG_LEVEL);
#endif
  log_trace("Trace enabled.");
  log_info("%s", "Initializing PIO for LEDs...");
  ws281x_pio_init();

  log_info("%s", "Initializing I2C for screen...");
  i2c_init(SCREEN_I2C, 400000);
  gpio_set_function(SCREEN_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCREEN_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(SCREEN_SDA_PIN);
  gpio_pull_up(SCREEN_SCL_PIN);

  /*
   * Start the tasks
   */
  xTaskCreate(rotary_encoder_task, "Rotary Encoders Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &tasks.rotary_encoders);

  xTaskCreate(buttons_task, "Buttons Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &tasks.buttons);

  xTaskCreate(context_screen_task, "Screen Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &tasks.screen);

  xTaskCreate(context_leds_task, "LEDs Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &tasks.leds);

  /* colors_menu_context_init(&menu_context, NULL); */
  /* context_enable(&menu_context); */

  rgb_encoders_context_init(&rgb_context, NULL, &rgb);
  context_enable(&rgb_context);

  vTaskStartScheduler();

  panic("This should not be reached.");
}

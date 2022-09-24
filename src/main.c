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
#include "task.h"

#include <stdio.h>
#if LIB_PICO_STDIO_USB && DEBUG
#include <tusb.h>
#endif

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "pico_debug.h"
#include "ws281x.h"
#include "pico_ft2.h"
#include "ssd1306.h"
#include "io_devices.h"


/*-----------------------------------------------------------*/

/*  So far, we're just tracking the state of the buttons */

static bool button_depressed[8];

/*-----------------------------------------------------------*/

/*  I/O RGB Encoder States -- this may evolve to an "object" */

static struct {
  bool active;
  uint8_t value;
  uint8_t shift;
  uint8_t button_offset;
} rgb_encoders[4];

static uint32_t rgb_encoders_value() {
  uint32_t rgb=0u;
  for (int i=0; i<4; i++) {
    if (!rgb_encoders[i].active) continue;
    rgb |= rgb_encoders[i].value << rgb_encoders[i].shift;
  }
  return rgb;
}

static void rgb_encoders_init() {
  rgb_encoders[ROTARY_ENCODER_RED_OFFSET].active = true;
  rgb_encoders[ROTARY_ENCODER_RED_OFFSET].shift = 16;
  rgb_encoders[ROTARY_ENCODER_RED_OFFSET].button_offset = BUTTON_RED_OFFSET;
  rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].active = true;
  rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].shift = 8;
  rgb_encoders[ROTARY_ENCODER_GREEN_OFFSET].button_offset = BUTTON_GREEN_OFFSET;
  rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].active = true;
  rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].shift = 0;
  rgb_encoders[ROTARY_ENCODER_BLUE_OFFSET].button_offset = BUTTON_BLUE_OFFSET;
}

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
 *  Atomic(ish) functions to update state values -- these
 *  will likely become context-dependent
 */

void process_rotary_encoder(uint8_t re_number, int8_t delta) {
  int16_t value = rgb_encoders[re_number].value;
  /* value += delta * ((~gpio_get_all() & (1<<BUTTON1_PIN|1<<BUTTON2_PIN)) ? 0x1u : 0x11u); */
  int8_t multiplier = button_depressed[rgb_encoders[re_number].button_offset] ? 0x1u : 0x11u;
  value += delta * multiplier;
  value = MAX(value, 0);
  value = MIN(value, 0xff);
  rgb_encoders[re_number].value = value;
};

void process_button(uint8_t button_number, int8_t new_state) {
  button_depressed[button_number] = new_state?true:false;
};

/*-----------------------------------------------------------*/

/*
 *  Tast definitions
 */

struct {
  TaskHandle_t rotary_encoders;
  TaskHandle_t buttons;
  TaskHandle_t screen;
  TaskHandle_t leds;
} tasks;

void task_rotary_encoders(void *parm) {
  /*
   * Initialization happens within the task because the notification task ID
   * can be easily detected.
   */
  io_devices_init_encoders(ROTARY_ENCODER_LOW_PIN, ROTARY_ENCODER_SM);

  uint32_t bits = 0u;
  for( ;; ) {
    for (int i=0; i<4; i++) {
      if(bits & 1u) {
        process_rotary_encoder(i, 1);
      }
      if(bits & 2u) {
        process_rotary_encoder(i, -1);
      }
      bits >>= 2;
    }
    xTaskNotifyIndexed(tasks.screen, 1, rgb_encoders_value(), eSetValueWithOverwrite);
    xTaskNotifyIndexed(tasks.leds, 1, rgb_encoders_value(), eSetValueWithOverwrite);

    if (!xTaskNotifyWaitIndexed(1, 0u, ULONG_MAX, &bits, portMAX_DELAY)) continue;
  }
}

void task_buttons(void *parm) {
  io_devices_init_buttons(BUTTON_LOW_PIN, BUTTON_SM);

  uint32_t bits = 0u;
  for( ;; ) {
    uint32_t bbits=bits;
    for (int i=0; i<8; i++) {
      if(bbits & 1u) {
        process_button(i, bbits & 2u);
      }
      bbits>>=2;
    }
    if (!xTaskNotifyWaitIndexed(1, 0u, ULONG_MAX, &bits, portMAX_DELAY)) continue;
  }
}

void task_leds(void * parm) {
  uint32_t rgb;
  for( ;; ) {
    if (!xTaskNotifyWaitIndexed( 1, 0u, ULONG_MAX, &rgb, portMAX_DELAY)) continue;
    ws281x_sparkle_pixels(1, &rgb);
  }
}

void task_screen(void *parm) {
  char hex_color_value[8];
  uint32_t rgb;
  ssd1306_t disp;

  /*  The screen needs a little time to warm up  */
  vTaskDelay(400 / portTICK_PERIOD_MS);

  debug_printf("%s", "Initializing Display...");
  disp.external_vcc=false;
  ssd1306_init(&disp, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_I2C_ADDRESS, SCREEN_I2C);
  ssd1306_clear(&disp);
  ssd1306_show(&disp);
  debug_printf("%s", "Display Initialized...");

  for( ;; ) {
    if (!xTaskNotifyWaitIndexed( 1, 0u, ULONG_MAX, &rgb, portMAX_DELAY)) continue;

    uint32_t pen_x, pen_y;
    pico_ft2_set_initial_pen_from_top_left(0, 0, &pen_x, &pen_y);

    sprintf(hex_color_value, "#%06lx", rgb);

    ssd1306_clear(&disp);
    for(int i=0; hex_color_value[i]; i++) {
      pico_ft2_render_char(&pen_x, &pen_y, hex_color_value[i], &disp,
          (pico_ft2_draw_function)ssd1306_draw_pixel);
    }
    ssd1306_show(&disp);
  }
}

int main() {
  stdio_init_all();
#if LIB_PICO_STDIO_USB && DEBUG
  while (!stdio_usb_connected()) { sleep_ms(100); }
#endif
  debug_printf("%s", "Initializing PIO...");
  ws281x_pio_init();

  debug_printf("%s", "Initializing Plex...");
  pico_ft2_init_otf();
  pico_ft2_set_font_size(12);

  debug_printf("%s", "Initializing Rotary Encoders...");
  rgb_encoders_init();
  io_devices_register_encoder(ROTARY_ENCODER_RED_OFFSET, ROTARY_ENCODER_RED_INVERTED);
  io_devices_register_encoder(ROTARY_ENCODER_GREEN_OFFSET, ROTARY_ENCODER_GREEN_INVERTED);
  io_devices_register_encoder(ROTARY_ENCODER_BLUE_OFFSET, ROTARY_ENCODER_BLUE_INVERTED);

  debug_printf("%s", "Initializing Buttons...");
  io_devices_register_button(BUTTON_UPPER_OFFSET);
  io_devices_register_button(BUTTON_LOWER_OFFSET);
  io_devices_register_button(BUTTON_RED_OFFSET);
  io_devices_register_button(BUTTON_GREEN_OFFSET);
  io_devices_register_button(BUTTON_BLUE_OFFSET);

  debug_printf("%s", "Initializing I2C...");
  i2c_init(SCREEN_I2C, 400000);
  gpio_set_function(SCREEN_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(SCREEN_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(SCREEN_SDA_PIN);
  gpio_pull_up(SCREEN_SCL_PIN);

  pico_ft2_set_font_size(12);

  /*
   * Start the three tasks
   */
  xTaskCreate(task_rotary_encoders, "Rotary Encoders Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &tasks.rotary_encoders);

  xTaskCreate(task_buttons, "Buttons Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, &tasks.buttons);

  /*
   * The current approach to using FreeType2 under the covers here leads to a
   * large demand for stack.  Some optimization may be in order, but not yet...
   */
  xTaskCreate(task_screen, "Screen Task",
      8192, NULL, tskIDLE_PRIORITY + 1, &tasks.screen);

  xTaskCreate(task_leds, "LEDs Task",
      configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &tasks.leds);

  vTaskStartScheduler();

  panic("This should not be reached.");
}

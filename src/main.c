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
#include "rotary_encoder.h"

#if PICO_ON_DEVICE
#include "pico/binary_info.h"
bi_decl(bi_program_description("Piano driver."));
#endif

static uint32_t rgb_value, prior_rgb_value = 0x1000000;

void process_rotary_encoder(uint8_t re_number, int8_t delta) {
  uint8_t shift = re_number<<3;
  int16_t value = rgb_value>>shift & 0xffu;
  value += delta * ((~gpio_get_all() & (1<<BUTTON1_PIN|1<<BUTTON2_PIN)) ? 0x1u : 0x11u);
  value = MAX(value, 0);
  value = MIN(value, 0xff);
  rgb_value &= ~(0xff<<shift);
  rgb_value |= ((uint8_t)value)<<shift;
};

int main() {
  stdio_init_all();
#if LIB_PICO_STDIO_USB && DEBUG
  //while (!stdio_usb_connected()) { sleep_ms(100);  }
#endif
  debug_printf("%s", "Initializing PIO...");
  ws281x_pio_init();

  debug_printf("%s", "Initializing I2C...");
  i2c_init(i2c1, 400000);
  gpio_set_function(18, GPIO_FUNC_I2C);
  gpio_set_function(19, GPIO_FUNC_I2C);
  gpio_pull_up(18);
  gpio_pull_up(19);

  debug_printf("%s", "Initializing Plex...");
  pico_ft2_init_otf();
  pico_ft2_set_font_size(12);

  debug_printf("%s", "Initializing Display...");
  ssd1306_t disp;
  disp.external_vcc=false;
  ssd1306_init(&disp, 128, 32, 0x3C, i2c1);
  ssd1306_clear(&disp);

  debug_printf("%s", "Initializing Rotary Encoders...");
  rotary_encoder_init(2, "Red Encoder", ROTARY_ENCODER_RED_LOW_PIN, ROTARY_ENCODER_RED_INVERTED, process_rotary_encoder);
  rotary_encoder_init(1, "Green Encoder", ROTARY_ENCODER_GREEN_LOW_PIN, ROTARY_ENCODER_GREEN_INVERTED, process_rotary_encoder);
  rotary_encoder_init(0, "Blue Encoder", ROTARY_ENCODER_BLUE_LOW_PIN, ROTARY_ENCODER_BLUE_INVERTED, process_rotary_encoder);

  debug_printf("%s", "Initializing Buttons...");
  gpio_set_input_enabled(BUTTON1_PIN, true);
  gpio_pull_up(BUTTON1_PIN);
  gpio_set_input_enabled(BUTTON2_PIN, true);
  gpio_pull_up(BUTTON2_PIN);

  char hex_color_value[8];
  pico_ft2_set_font_size(14);

  while(true) {
    if(rgb_value == prior_rgb_value) continue;

    uint32_t pen_x, pen_y;
    pico_ft2_set_initial_pen_from_top_left(0, 0, &pen_x, &pen_y);

    sprintf(hex_color_value, "#%06lx", rgb_value);

    ssd1306_clear(&disp);
    for(int i=0; hex_color_value[i]; i++) {
      pico_ft2_render_char(&pen_x, &pen_y, hex_color_value[i], &disp,
          (pico_ft2_draw_function)ssd1306_draw_pixel);
    }
    ssd1306_show(&disp);

    ws281x_sparkle_pixels(1, &rgb_value);
    prior_rgb_value = rgb_value;
  }
}

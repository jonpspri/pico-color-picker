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

#include "pico_debug.h"
#include "ws281x.h"
#include "pico_ft2.h"

#if PICO_ON_DEVICE
#include "pico/binary_info.h"
bi_decl(bi_program_description("Piano driver."));
#endif

int main() {

  stdio_init_all();
#if LIB_PICO_STDIO_USB && DEBUG
  while (!tud_cdc_connected()) { sleep_ms(100);  }
#endif
  debug_printf("%s", "Initializing PIO...");
  //ws281x_pio_init();

  debug_printf("%s", "Initializing Plex...");
  pico_ft2_init_otf();

  uint16_t font_size = 16;
  debug_printf("Setting font size to %d", font_size);
  pico_ft2_set_font_size(16);

  const char render_string[] = "IBM";
  debug_printf("Attempting to render %s", render_string);

  for(int i=0; render_string[i]; i++) pico_ft2_render_char(render_string[i]);

  while(true) {
    sleep_ms(10000);
  }
}

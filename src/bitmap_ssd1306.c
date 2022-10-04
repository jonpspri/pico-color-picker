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

#include <string.h>

#include "pico/stdlib.h"

#include "bitmap.h"
#include "ssd1306.h"

static void b_ssd1306_clear(bitmap_t *b) {
  ssd1306_clear((ssd1306_t *)b->buffer);
}

static void b_ssd1306_draw_pixel(bitmap_t *b, uint32_t x, uint32_t y, bool value) {
  ssd1306_draw_pixel((ssd1306_t *)b->buffer, x, y, value);
}

static bool b_ssd1306_pixel_value(bitmap_t *b, uint32_t x, uint32_t y) {
  return ssd1306_pixel_value((ssd1306_t *)b->buffer, x, y);
}

static void b_ssd1306_free_buffer() {
  panic("Not implemented :(");
}

void b_ssd1306_init(bitmap_t *b) {
  ssd1306_t *disp = memset(pvPortMalloc(sizeof(ssd1306_t)),0,sizeof(ssd1306_t));

  disp->external_vcc = false;

  ssd1306_init(disp, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_I2C_ADDRESS, SCREEN_I2C);

  b->clear = b_ssd1306_clear;
  b->draw_pixel = b_ssd1306_draw_pixel;
  b->pixel_value = b_ssd1306_pixel_value;
  b->free_buffer = b_ssd1306_free_buffer;

  b->buffer=disp;
}

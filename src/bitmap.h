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


#ifndef __BITMAP_H
#define __BITMAP_H

#include "pico/stdlib.h"

#include "fonts/font.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "ssd1306.h"

#ifdef __cplusplus
extern "C" {
#endif


struct bitmap_buffer;
typedef struct bitmap_buffer bitmap_buffer_t;
struct bitmap_buffer {
  void (*draw_pixel)(bitmap_buffer_t *, uint32_t x, uint32_t y, bool value);
  bool (*pixel_value)(bitmap_buffer_t *, uint32_t x, uint32_t y);
  void (*clear)(bitmap_buffer_t *);
  void (*free_buffer)(bitmap_buffer_t *);
  void *d;
};

typedef struct bitmap {
  uint32_t width;
  uint32_t height;
  uint32_t words_per_line;
  bool inverted;
  uint32_t *buffer;
} bitmap_t;

bitmap_t *bitmap_init(uint32_t width, uint32_t height);

void bitmap_clear(bitmap_t *);
void bitmap_draw_pixel(bitmap_t *, uint32_t x, uint32_t y, bool value);
void bitmap_draw_char(bitmap_t *, uint32_t x, uint32_t y, const struct bitmap_font *font, uint16_t c);
void bitmap_draw_string(bitmap_t *, uint32_t x, uint32_t y, const struct bitmap_font *font, const char *);
void bitmap_invert(bitmap_t *);
void bitmap_copy_from(bitmap_t *, bitmap_t *);  // Temporary ?
void bitmap_rerender(bitmap_t *, uint32_t x_max, uint32_t y_max, void (* callback)(void *, uint32_t, uint32_t, bool), void *target);

#ifdef __cplusplus
}
#endif

#endif /* __BITMAP_H */

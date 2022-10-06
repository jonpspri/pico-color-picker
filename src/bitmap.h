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

#ifdef __cplusplus
extern "C" {
#endif

struct bitmap;
typedef struct bitmap bitmap_t;

struct bitmap {
  uint32_t width;
  uint32_t height;
  uint32_t words_per_line;
  bool inverted;

  void (*draw_pixel)(bitmap_t *, uint32_t x, uint32_t y, bool value);
  bool (*pixel_value)(bitmap_t *, uint32_t x, uint32_t y);
  void (*clear)(bitmap_t *);
  void (*free_buffer)(bitmap_t *);

  void *buffer;
};

bool bitmap_init(bitmap_t *b, uint32_t width, uint32_t height, void (*custom_init)(bitmap_t *));
void bitmap_free(bitmap_t *bitmap);

void bitmap_draw_char(bitmap_t *, uint32_t x, uint32_t y, const struct bitmap_font *font, uint16_t c);
void bitmap_draw_empty_square(bitmap_t *, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void bitmap_draw_square(bitmap_t *, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void bitmap_draw_string(bitmap_t *, uint32_t x, uint32_t y, const struct bitmap_font *font, const char *);

void bitmap_copy_from(bitmap_t *, bitmap_t *, uint32_t x, uint32_t y);

static inline void bitmap_invert(bitmap_t *b) { b->inverted = !b->inverted; }
static inline void bitmap_clear(bitmap_t *b) { b->inverted = false; b->clear(b); }
static inline void bitmap_draw_pixel(bitmap_t *b, uint32_t x, uint32_t y, bool value) { b->draw_pixel(b, x, y, value); }
static inline bool bitmap_pixel_value(bitmap_t *b, uint32_t x, uint32_t y) { return b->pixel_value(b, x, y); }

#ifdef __cplusplus
}
#endif

#endif /* __BITMAP_H */

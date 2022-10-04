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

#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "log.h"
#include "bitmap.h"

#include "fonts/font.h"

/* ---------------------------------------------------------------------- */

static int compare_uint16_t (const void * a, const void * b) {
  return ( *(uint16_t *)a - *(uint16_t*)b );
}

static void draw_pixel_callback(void *bitmap, uint32_t x, uint32_t y, bool value) {
  bitmap_draw_pixel((bitmap_t *)bitmap, x, y, value);
}

/* ---------------------------------------------------------------------- */

#define WORDS(_x) (((_x)-1)/32+1)

static void w_draw_pixel(bitmap_t *b, uint32_t x, uint32_t y, bool value) {
  if (x > b->width || y > b->height) {
    log_error("Drawing point (%d, %d) out of bound for bitmap (%d, %d)", x, y, b->width, b->height);
    return;
  }
  uint32_t *w = &(((uint32_t *)b->buffer)[y * WORDS(b->width) + (x>>5)]);
  if (value) { *w |=  1<<(31-(x & 31u)); }
  else { *w &= ~(1<<(31-(x & 31u))); }
}

static bool w_pixel_value(bitmap_t *b, uint32_t x, uint32_t y) {
  bool pixel = ((uint32_t*)b->buffer)[y * WORDS(b->width) + (x>>5)] & 1<<(31-(x&31u));
  return ( b->inverted != pixel );  /*  Essentially an XOR  */
}

static void w_clear(bitmap_t *b) {
  memset(b->buffer, 0, b->height * WORDS(b->width) * 4);
}

static void w_free_buffer(bitmap_t *b) {
  vPortFree(b->buffer);
}

/* ---------------------------------------------------------------------- */

bitmap_t *bitmap_init(uint32_t width, uint32_t height) {
  bitmap_t *b= pvPortMalloc(sizeof(bitmap_t));
  b->width = width;
  b->height = height;
  b->inverted = false;

  b->draw_pixel = w_draw_pixel;
  b->pixel_value = w_pixel_value;
  b->clear = w_clear;
  b->free_buffer = w_free_buffer;

  uint32_t b_size = height * WORDS(width) * 4;
  b->buffer = memset(pvPortMalloc(b_size),0,b_size);
  return b;
}

void bitmap_free(bitmap_t *bitmap) {
  vPortFree(bitmap->buffer);
  vPortFree(bitmap);
}

void bitmap_clear(bitmap_t *b) {
  b->clear(b);
}

void bitmap_draw_pixel(bitmap_t *b, uint32_t x, uint32_t y, bool value) {
  b->draw_pixel(b, x, y, value);
}

bool bitmap_pixel_value(bitmap_t *b, uint32_t x, uint32_t y) {
  return b->pixel_value(b, x, y);
}

/*
void bitmap_rotate_ccw(bitmap_t *b, bitmap_t *source, uint32_t x, uint32_t y) {
  for (uint32_t i=0; i<source->width; i++) {
    for (uint32_t j=0; j<source->height; j++) {
      draw_pixel(x+j, y-i, bitmap_pixel_value(source, i, j));
    }
  }
}
*/

void bitmap_invert(bitmap_t *b) {
  b->inverted = !b->inverted;
}

void bitmap_copy_from(bitmap_t *b, bitmap_t *source) {
  bitmap_rerender(source, source->width, source->height, draw_pixel_callback, b);
}

void bitmap_rerender(bitmap_t *b, uint32_t x_max, uint32_t y_max,
  void (* callback)(void *, uint32_t, uint32_t, bool), void *target) {
  for (uint32_t i=0; i<x_max && i<b->width; i++) {
    for (uint32_t j=0; j<y_max && j<b->height; j++) {
      if (bitmap_pixel_value(b, i, j)) { callback(target, i, j, true); }
    }
  }
}

#if 0
void bitmap_draw_5x5_checkbox(bitmap_t *b, uint32_t x, uint32_t y, bool checked) {
  uint32_t i, j;
  if(checked) {
    /* Draw a solid box */
    for (i=0; i<5; i++) {
      for (j=0; j<5; j++) {
        bitmap_draw_pixel(b, x+i, y+j, 1);
      }
    }
  } else {
    /* Use four lines (and overlap at the corners) to draw a box. */
    for (i=0; i<5; i++) {
      bitmap_draw_pixel(b, x+i, y, 1);
      bitmap_draw_pixel(b, x+i, y+4, 1);
      bitmap_draw_pixel(b, x, y+i, 1);
      bitmap_draw_pixel(b, x+4, y+i, 1);
    }
    /* Clear the innards */
    for (i=0; i<3; i++) {
      for (j=0; j<3; j++) {
        bitmap_draw_pixel(b, x+i+1, y+j+1, 0);
      }
    }
  }
}
#endif

void bitmap_draw_char(bitmap_t *b, uint32_t x, uint32_t y, const struct bitmap_font *font, uint16_t c) {
  uint16_t *c_index_ptr = (uint16_t *)
    bsearch(&c, font->Index, font->Chars, sizeof(uint16_t), compare_uint16_t);
  if(!c_index_ptr) {
    log_error("Character %d not found in font.", c);
  }
  uint16_t span = (font->Width-1)/8 + 1;
  uint16_t bitmap_offset = (c_index_ptr - font->Index)*font->Height*span;

  for(uint32_t i=0; i<font->Height; i++) {
    const uint8_t *line=&font->Bitmap[bitmap_offset + i*span];

    for(int32_t j=0; j<font->Width; j++)  {
      if(line[j>>3] & 1<<(7-(j&7u))) {
        bitmap_draw_pixel(b, x+j, y+i, true);
      }
    }
  }
}

void bitmap_draw_string(bitmap_t *b, uint32_t x, uint32_t y, const struct bitmap_font *font, const char *string) {
  for(int i=0; i<strlen(string); i++) {
    bitmap_draw_char(b, x+i*font->Width, y, font, string[i]);
  }
}

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

#include "fonts/font.h"
#include "pico/stdlib.h"
#include "log.h"
#include "bitmap.h"

#include <stdlib.h>
#include <string.h>

Bitmap::Bitmap(uint32_t width_, uint32_t height_) :
      width(width_),
      height(height_),
      words_per_line((width_-1)/32+1),
      buffer((uint32_t *)pvPortMalloc(height*words_per_line*sizeof(uint32_t))),
      inverted(false)
{ }

Bitmap::~Bitmap() {
      vPortFree(buffer);
}

void Bitmap::clear() {
      memset(buffer, 0, words_per_line * height * 4);
}

// TODO:  Create "Pixel" subclass that manages mapping of Pixels to buffer
void Bitmap::draw_pixel(uint32_t x, uint32_t y, bool value) {
  if (x > width || y > height) {
    log_error("Drawing point (%d, %d) out of bound for bitmap (%d, %d)", x, y, width, height);
  }
  if (value) {
    buffer[y*words_per_line + (x>>5)] |= 1<<(31-(x & 31u));
  } else {
    buffer[y*words_per_line + (x>>5)] &= ~(1<<(31-(x & 31u)));
  }
}

bool Bitmap::pixel_value(uint32_t x, uint32_t y) {
  bool pixel = buffer[y*words_per_line + (x>>5)] & 1<<(31-(x&31u));
  return ( inverted != pixel );  /*  Essentially an XOR  */
}

void Bitmap::draw_char(uint32_t x, uint32_t y, const struct bitmap_font *font, uint16_t c) {
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
            draw_pixel(x+j, y+i, true);
          }
        }
      }
    }

void Bitmap::draw_string(uint32_t x, uint32_t y, const struct bitmap_font *font, char *string) {
      for(int i=0; i<strlen(string); i++) {
       draw_char(x+i*font->Width, y, font, string[i]);
      }
    }

void Bitmap::rotate_ccw(Bitmap &source, uint32_t x, uint32_t y) {
      for (uint32_t i=0; i<source.width; i++) {
        for (uint32_t j=0; j<source.height; j++) {
          draw_pixel(x+j, y-i, source.pixel_value(i, j));
        }
      }
    }

void Bitmap::invert() {
  inverted = inverted?0:1;
}

void Bitmap::rerender(uint32_t x_max, uint32_t y_max,
  void (* callback)(void *, uint32_t, uint32_t), void *target) {
  for (uint32_t i=0; i<x_max && i<width; i++) {
    for (uint32_t j=0; j<y_max && j<height; j++) {
      if (pixel_value(i, j)) { callback(target, i, j); }
    }
  }
}

void Bitmap::copy_from(Bitmap &source) {
  source.rerender(width, height, bitmap_draw_pixel_callback, this);
}

void bitmap_draw_5x5_checkbox(Bitmap &bh, uint32_t x, uint32_t y, bool checked) {
  uint32_t i, j;
  if(checked) {
    /* Draw a solid box */
    for (i=0; i<5; i++) {
      for (j=0; j<5; j++) {
        bh.draw_pixel(x+i, y+j, 1);
      }
    }
  } else {
    /* Use four lines (and overlap at the corners) to draw a box. */
    for (i=0; i<5; i++) {
      bh.draw_pixel(x+i, y, 1);
      bh.draw_pixel(x+i, y+4, 1);
      bh.draw_pixel(x, y+i, 1);
      bh.draw_pixel(x+4, y+i, 1);
    }
    /* Clear the innards */
    for (i=0; i<3; i++) {
      for (j=0; j<3; j++) {
        bh.draw_pixel(x+i+1, y+j+1, 0);
      }
    }
  }
}

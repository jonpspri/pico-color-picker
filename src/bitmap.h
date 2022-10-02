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


class Bitmap {
  private:
    // TODO:  Move a lot of this into the "Pixel" interface
    const uint32_t words_per_line;
    uint32_t *buffer;
    bool inverted;
    static int compare_uint16_t (const void * a, const void * b) {
      return ( *(uint16_t *)a - *(uint16_t*)b );
    }
    // TODO:  Create a "drawable" interface to rework this.
    static void bitmap_draw_pixel_callback(void *bitmap, uint32_t x, uint32_t y) {
      static_cast<Bitmap *>(bitmap)->draw_pixel(x, y, true);
    }
  public:
    const uint32_t width;
    const uint32_t height;

    Bitmap(uint32_t, uint32_t);
    ~Bitmap();

    void clear();
    void draw_pixel(uint32_t, uint32_t, bool);
    bool pixel_value(uint32_t, uint32_t);
    void draw_char(uint32_t, uint32_t, const struct bitmap_font *font, uint16_t);
    void draw_string(uint32_t, uint32_t, const struct bitmap_font *font, char *);
    void rotate_ccw(Bitmap &, uint32_t, uint32_t);
    void invert();
    // TODO:  Rework to be more interface-y (or a copy?)
    void rerender(uint32_t x_max, uint32_t y_max, void (* callback)(void *, uint32_t, uint32_t), void *target);

    void copy_from(Bitmap &source);
};

#ifdef __cplusplus
}
#endif

#endif /* __BITMAP_H */

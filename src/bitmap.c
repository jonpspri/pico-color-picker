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

/** @file bitmap.c
 *
 */

#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"

#include "log.h"
#include "bitmap.h"

#include "fonts/font.h"

/* ---------------------------------------------------------------------- */

static int compare_uint16_t (const void *a, const void *b)
{
    return ( *(uint16_t *) a - *(uint16_t *) b );
}

/* ---------------------------------------------------------------------- */

#define WORDS(_x) ( ( ( _x ) - 1 ) / 32 + 1 )

static void w_draw_pixel(bitmap_t *b, uint32_t x, uint32_t y, bool value)
{
    if ( ( x > b->width ) || ( y > b->height ) ) {
        log_error("Drawing point (%d, %d) out of bound for bitmap (%d, %d)",
                x,
                y,
                b->width,
                b->height
                );
        return;
    }
    uint32_t *w = &( ( (uint32_t *) b->buffer )[y * WORDS(b->width) + ( x >> 5 )] );
    if (value) {
        *w |= 1 << ( 31 - ( x & 31u ) );
    }else {
        *w &= ~( 1 << ( 31 - ( x & 31u ) ) );
    }
} /* w_draw_pixel */

static bool w_pixel_value(bitmap_t *b, uint32_t x, uint32_t y)
{
    bool pixel = ( (uint32_t *) b->buffer )[y * WORDS(b->width) + ( x >> 5 )] & 1 <<
    ( 31 - ( x & 31u ) );
    return ( b->inverted != pixel ); /*  Essentially an XOR  */
}

static void w_clear(bitmap_t *b)
{
    memset(b->buffer, 0, b->height * WORDS(b->width) * 4);
}

static void w_free_buffer(bitmap_t *b)
{
    vPortFree(b->buffer);
}

/* ---------------------------------------------------------------------- */

void bitmap_free(void *v)
{
    bitmap_t *b = (bitmap_t *) v;
    ASSERT_IS_A(b, BITMAP_T);

    if (b->free_buffer) {
        b->free_buffer(b);
    } else {
        if (b->buffer) {
            vPortFree(b->buffer);
        }
    }
    vPortFree(v);
} /* bitmap_free */

/** @brief Initialize a bitmap if needed
 *  @param b The \ref bitmap_t to be initialized
 *  @param width Width of the new bitmap
 *  @param height Height of the new bitmap
 *  @param custom_init A custom initializer hook for the bitmap, or `NULL` for a standard bitmap.
 *  @return `true` if initialization was required.
 */
bitmap_t *bitmap_alloc( uint32_t width, uint32_t height, void ( *custom_init )(bitmap_t *) )
{
    bitmap_t *b = pcp_zero_malloc( sizeof( bitmap_t ) );
    b->pcp.magic_number = BITMAP_T | FREEABLE_P;
    b->pcp.free_f = bitmap_free;
    b->pcp.autofree_p = true;

    b->width = width;
    b->height = height;
    b->inverted = false;

    if (!custom_init) {
        b->draw_pixel = w_draw_pixel;
        b->pixel_value = w_pixel_value;
        b->clear = w_clear;
        b->free_buffer = w_free_buffer;
        uint32_t b_size = height * WORDS(width) * 4;
        b->buffer = memset(pvPortMalloc(b_size),0,b_size);
    } else {
        custom_init(b);
    }
    return b;
} /* bitmap_alloc */

void bitmap_copy_from_bound(bitmap_t *b,
        bitmap_t *source,
        uint32_t x,
        uint32_t y,
        uint32_t width,
        uint32_t height)
{
    for (int i = 0; i<width; i++) {
        for (int j = 0; j<height; j++) {
            bitmap_draw_pixel( b, i + x, j + y, bitmap_pixel_value(source, i, j) );
        }
    }
}

void bitmap_copy_from(bitmap_t *b, bitmap_t *source, uint32_t x, uint32_t y)
{
    bitmap_copy_from_bound(b, source, x, y,
            MIN(b->width, source->width), MIN(b->height, source->height)
            );
}

void bitmap_draw_char(bitmap_t *b,
        uint32_t x,
        uint32_t y,
        const struct bitmap_font *font,
        uint16_t c)
{
    uint16_t *c_index_ptr = (uint16_t *)
                            bsearch(&c,
            font->Index,
            font->Chars,
            sizeof( uint16_t ),
            compare_uint16_t
            );
    if (!c_index_ptr) {
        log_error("Character %d not found in font.", c);
    }
    uint16_t span = ( font->Width - 1 ) / 8 + 1;
    uint16_t bitmap_offset = ( c_index_ptr - font->Index ) * font->Height * span;

    for (uint32_t i = 0; i<font->Height; i++) {
        const uint8_t *line = &font->Bitmap[bitmap_offset + i * span];

        for (int32_t j = 0; j<font->Width; j++)  {
            if ( line[j >> 3] & 1 << ( 7 - ( j & 7u ) ) ) {
                bitmap_draw_pixel(b, x + j, y + i, true);
            }
        }
    }
} /* bitmap_draw_char */

void bitmap_draw_string(bitmap_t *b,
        uint32_t x,
        uint32_t y,
        const struct bitmap_font *font,
        const char *string)
{
    for (int i = 0; i<strlen(string); i++) {
        bitmap_draw_char(b, x + i * font->Width, y, font, string[i]);
    }
}

inline static void swap(uint32_t *a, uint32_t *b)
{
    uint32_t *t = a;
    *a = *b;
    *b = *t;
}

void bitmap_draw_line(bitmap_t *p, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    if (x1>x2) {
        swap(&x1, &x2);
        swap(&y1, &y2);
    }

    if (x1==x2) {
        if (y1>y2) {
            swap(&y1, &y2);
        }
        for (uint32_t i = y1; i<=y2; ++i) {
            bitmap_draw_pixel(p, x1, i, true);
        }
        return;
    }

    float m = (float) ( y2 - y1 ) / (float) ( x2 - x1 );

    for (uint32_t i = x1; i<=x2; ++i) {
        float y = m * (float) ( i - x1 ) + (float) y1;
        bitmap_draw_pixel(p, i, y, true);
    }
} /* bitmap_draw_line */

void bitmap_draw_square(bitmap_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i<width; ++i) {
        for (uint32_t j = 0; j<height; ++j) {
            bitmap_draw_pixel(p, x + i, y + j, true);
        }
    }
}

void bitmap_draw_empty_square(bitmap_t *p, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    bitmap_draw_line(p, x, y, x + width, y);
    bitmap_draw_line(p, x, y + height, x + width, y + height);
    bitmap_draw_line(p, x, y, x, y + height);
    bitmap_draw_line(p, x + width, y, x + width, y + height);
}

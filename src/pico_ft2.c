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

#include <ft2build.h>
#include <freetype/freetype.h>

#include "pico_debug.h"

extern FT_Byte _binary_IBMPlexMono_Regular_otf_start;
extern uint32_t _binary_IBMPlexMono_Regular_otf_size;


static FT_Library ft_library;
static FT_Face ft_face;

void pico_ft2_init_otf() {
    FT_Error error;
    debug_printf("%s", "FT_Init_FreeType...");
    error = FT_Init_FreeType( &ft_library );
    if(error) {
        debug_printf("FT_Init_FreeType failed: %s", FT_Error_String(error));
    }

    debug_printf("%s", "FT_New_Memory_Face...");
    error = FT_New_Memory_Face( ft_library,
        &_binary_IBMPlexMono_Regular_otf_start,
        (FT_Long) &_binary_IBMPlexMono_Regular_otf_size,
        0, &ft_face );
    if(error) {
        debug_printf("FT_New_Memory_Face failed: %s", FT_Error_String(error));
    }
}

void pico_ft2_set_font_size(FT_Long size) {
    FT_Error error;
    error = FT_Set_Pixel_Sizes( ft_face, 0, size );
    if(error) {
        debug_printf("FT_New_Memory_Face failed: %s", FT_Error_String(error));
    }
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

void pico_ft2_render_char(FT_ULong char_code) {
    FT_Load_Char( ft_face, char_code,
        FT_LOAD_RENDER | FT_LOAD_MONOCHROME
        );
    FT_Bitmap bitmap = ft_face->glyph->bitmap;
    for (int row=0; row<bitmap.rows; row++) {
      for (int b=0; b<bitmap.pitch; b++) {
        printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(bitmap.buffer[row*bitmap.pitch+b]));
      }
      putchar('\n');
      fflush(stdout);
    }
}

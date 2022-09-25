/*
 * SPDX-FileCopyrightText: 2022 Jonathan Springer
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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

#include "log.h"
#include "ssd1306.h"
#include "pico_ft2.h"

/*------------------------------------------------------------------------------
 *
 * FreeType 2 definitions here, including a couple fonts that have been inlined
 *
 * Some notes on IBM Plex Mono sizing:
 *    12 Point seems appropriate for 16-pixel high text (2 lines on a 128x32 display)
 */

extern FT_Byte _binary_IBMPlexMono_Regular_otf_start;
extern uint32_t _binary_IBMPlexMono_Regular_otf_size;
extern FT_Byte _binary_IBMPlexMono_Light_otf_start;
extern uint32_t _binary_IBMPlexMono_Light_otf_size;

static FT_Library ft_library;
static FT_Face ft_face;

/*------------------------------------------------------------------------------
 *
 * TODO:  Some form of Glyph caching.
 *        Parameterized "Face" and multiple initialized faces.
 */
void pico_ft2_init_otf() {
    FT_Error error;
    error = FT_Init_FreeType( &ft_library );
    if(error) log_error("FT_Init_FreeType failed: %s", FT_Error_String(error));

    error = FT_New_Memory_Face( ft_library,
        &_binary_IBMPlexMono_Light_otf_start,
        (FT_Long) &_binary_IBMPlexMono_Light_otf_size,
        0, &ft_face );
    if(error) log_error("FT_New_Memory_Face failed: %s", FT_Error_String(error));

    log_info("Loaded %s %s", ft_face->family_name, ft_face->style_name);
    log_info("Units per em: %d", ft_face->units_per_EM);
    log_info("Bounding Box: (%d,%d)-(%d,%d)",
            ft_face->bbox.xMin, ft_face->bbox.yMin,
            ft_face->bbox.xMax, ft_face->bbox.yMax);
}

void pico_ft2_set_font_size(FT_Long size) {
    FT_Size_RequestRec ft_size_request = {
        FT_SIZE_REQUEST_TYPE_CELL,
        size << 6, size << 6, 145, 145
    };

    FT_Error error;
    log_info("Requesting font size %ld", size);
    error = FT_Request_Size(ft_face, &ft_size_request);
    if(error) log_error("FT_Request_Size failed: %s", FT_Error_String(error));
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? 'X' : ' '), \
  (byte & 0x40 ? 'X' : ' '), \
  (byte & 0x20 ? 'X' : ' '), \
  (byte & 0x10 ? 'X' : ' '), \
  (byte & 0x08 ? 'X' : ' '), \
  (byte & 0x04 ? 'X' : ' '), \
  (byte & 0x02 ? 'X' : ' '), \
  (byte & 0x01 ? 'X' : ' ')

void pico_ft2_render_char(uint32_t *pen_x, uint32_t *pen_y, FT_ULong char_code, void *canvas, pico_ft2_draw_function draw) {
    FT_Error error;
    error = FT_Load_Char( ft_face, char_code,
            FT_LOAD_RENDER | FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO);
    if(error) log_error("FT_Load_Char failed: %s", FT_Error_String(error));

    log_trace("Rendering char %02x; width is %d", char_code, ft_face->glyph->bitmap.width);

    for (uint32_t row=0; row < ft_face->glyph->bitmap.rows; row++) {
        unsigned char *row_bytes = &(ft_face->glyph->bitmap.buffer[row*ft_face->glyph->bitmap.pitch]);
#if FT2_DEBUG
        for (int i=0; i<ft_face->glyph->bitmap.pitch; i++)
            fprintf(stderr, BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(row_bytes[i]));
        printf("\n");
#endif
        for (int col=0; col < ft_face->glyph->bitmap.width; col++) {
            if(row_bytes[col>>3] & (1<<(7-(col&0x7u)))) {
                uint32_t x = *pen_x + ft_face->glyph->bitmap_left + col;
                uint32_t y = *pen_y - ft_face->glyph->bitmap_top + row;
                draw(canvas, x, y);
            }
        }
    }
    (*pen_x) += ft_face->glyph->advance.x>>6;
    (*pen_y) += ft_face->glyph->advance.y>>6;
}

/*------------------------------------------------------------------------------*
 *
 * TO DO:  There has to be some more deterministic way to do this based on the
 *         face, doesn't there?
 */
void pico_ft2_set_initial_pen_from_top_left(uint32_t x, uint32_t y, uint32_t *pen_x, uint32_t *pen_y) {
    FT_Error error;
    error = FT_Load_Char( ft_face, 'l', FT_LOAD_RENDER | FT_LOAD_MONOCHROME | FT_LOAD_TARGET_MONO);
    if(error) log_error("FT_Load_Char failed: %s", FT_Error_String(error));

    *pen_x = x;
    *pen_y = y + ft_face->glyph->bitmap_top;
}

uint32_t pico_ft2_line_height_px() {
    return ft_face->size->metrics.height>>6;
}

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
#ifndef __PICO_FT2_H
#define __PICO_FT2_H

#include <ft2build.h>
#include <freetype/freetype.h>

typedef void (*pico_ft2_draw_function)(void *, uint32_t, uint32_t);

extern void pico_ft2_init_otf();
extern void pico_ft2_set_font_size(FT_Long);
extern void pico_ft2_render_char(uint32_t *, uint32_t *, FT_ULong, void *, pico_ft2_draw_function);
extern void pico_ft2_set_initial_pen_from_top_left(uint32_t, uint32_t, uint32_t*, uint32_t*);
extern uint32_t pico_ft2_line_height_px();
#endif
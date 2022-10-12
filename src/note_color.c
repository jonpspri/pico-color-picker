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

/** @file color.c
 *  @brief Manager the notes to colors mapping, both "permanent" and transient as it
 *         is manipulated through the color menu and through the chord editor.
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "pcp.h"
#include "log.h"

#include "context.h"
#include "note_color.h"

struct note_color {
  uint32_t magic_number;
  const char *note_name;
  uint32_t rgb;
  context_t rgb_encoder;
};

static const char *initial_names[12] = {
  "C", "C#/Db", "D", "D#/Eb", "E", "F",
  "F#/Gb", "G", "G#/Ab", "A", "A#/B#", "B"
};

static uint32_t initial_rgbs[12] = {
0xFF0000 , 0xcc1100 , 0xbb2200 , 0xcc5500 , 0xffcc00 , 0x33ff00 ,
0x00cd71 , 0x008AA1 , 0x2161b0 , 0x2200ff , 0x860e90 , 0xB8154A
};

static note_color_t note_colors[12];

/* ----------------------------------------------------------------------
 * Callbacks
 */

/* ---------------------------------------------------------------------- */
const char *note_color_note_name_i(uint8_t i) {
  assert(i<12);
  return note_colors[i].note_name;
}

const char *note_color_note_name(note_color_t *n) {
  return n->note_name;
}

uint32_t *note_color_rgb_i(uint8_t i) {
  assert(i<12);
  return &note_colors[i].rgb;
}

uint32_t *note_color_rgb(note_color_t *n) {
  return &n->rgb;
}

note_color_t *note_color_ptr_i(uint8_t i) {
  assert(i<12);
  return &note_colors[i];
}

void note_color_init() {
  for(uint8_t i=0; i<12; i++) {
    note_colors[i].magic_number = NOTE_COLOR_T;
    note_colors[i].note_name = initial_names[i];
    note_colors[i].rgb = initial_rgbs[i];
  }
}

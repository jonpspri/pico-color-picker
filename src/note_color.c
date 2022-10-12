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

#include "note_color.h"

struct note_color {
  uint32_t magic_number;
  const char *note_name;
  uint32_t rgb;
};

static const note_color_t initial_note_colors[12] = {
  { NOTE_COLOR_T, "C", 0xFF0000 },     { NOTE_COLOR_T, "C#/Db", 0xcc1100 },
  { NOTE_COLOR_T, "D", 0xbb2200 },     { NOTE_COLOR_T, "D#/Eb", 0xcc5500 },
  { NOTE_COLOR_T, "E", 0xffcc00 },     { NOTE_COLOR_T, "F", 0x33ff00 },
  { NOTE_COLOR_T, "F#/Gb", 0x00cd71 }, { NOTE_COLOR_T, "G", 0x008AA1 },
  { NOTE_COLOR_T, "G#/Ab", 0x2161b0 }, { NOTE_COLOR_T, "A", 0x2200ff },
  { NOTE_COLOR_T, "A#/B#", 0x860e90 }, { NOTE_COLOR_T, "B", 0xB8154A }
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
  memcpy(note_colors, initial_note_colors, sizeof(note_colors));
}

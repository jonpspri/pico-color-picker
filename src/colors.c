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

#include "pico/stdlib.h"
#include "context.h"

static struct {
  char *note_name;
  uint32_t color;
} note_colors[] = {
  { "C", 0xFF0000 },
  { "C#/Db", 0xcc1100 },
  { "D", 0xbb2200 },
  { "D#/Eb", 0xcc5500 },
  { "E", 0xffcc00 },
  { "F", 0x33ff00 },
  { "F#/Gb", 0x00cd71 },
  { "G", 0x008AA1 },
  { "G#/Ab", 0x2161b0 },
  { "A", 0x2200ff },
  { "A#/B#", 0x860e90 },
  { "B", 0xB8154A }
};

static uint8_t cursor_position;

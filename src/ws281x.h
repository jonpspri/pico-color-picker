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

#ifndef __WS281X_H
#define __WS281X_H

#include "pico/stdlib.h"

#ifndef USE_WS2812
#define USE_WS2812 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void ws2812_put_pixels(uint32_t **rgbs, uint8_t size);
extern void ws281x_sparkle_pixels(uint16_t rgb_count, uint32_t *rgbs);
extern void ws281x_pio_init();

#ifdef __cplusplus
}
#endif

#endif /* __WS281X_H */

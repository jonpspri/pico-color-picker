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

#ifndef __RGB_ENCODERS_H
#define __RGB_ENCODERS_H

#include "context.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OPAQUE */
typedef struct rgb_encoders_data rgb_encoders_data_t;

bool rgb_encoders_context_init(context_t *, context_t *parent, uint32_t *rgb);

#ifdef __cplusplus
}
#endif

#endif

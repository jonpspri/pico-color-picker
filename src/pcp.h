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

#ifndef __PCP_H
#define __PCP_H

/** @file php.h
 *
 *  @brief Pico Color Picker system-wide header file
 */

#include <string.h>

#include "FreeRTOS.h"

/* ----------------------------------------------------------------------- */

/* MAGIC NUMBERS */

#define UINITIALIZED        0x00
#define CONTEXT_SCREEN_T    0x01
#define RGB_ENCODER_T       0x02
#define RGB_ENCODERS_DATA_T 0x03
#define CONTEXT_T           0x04
#define CONTEXT_LEDS_T      0x05

/* THREAD_LOCAL_STORAGE */
#define TH_LOC_ST_CALLBACKS       0

/* NOTIFICATION INDICES */
#define NFCN_IDX_EVENT            1
/* -- 2 has mutually exlusive for different tasks */
#define NFCN_IDX_RGBS             2
#define NFCN_IDX_CONTEXT          2

/* ----------------------------------------------------------------------- */

static inline void *pcp_zero_malloc(size_t s) { return memset(pvPortMalloc(s),0,s); }

#endif /* __PCP_H */

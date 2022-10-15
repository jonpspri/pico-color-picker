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

#include <string.h>

#include "FreeRTOS.h"

#ifndef __PCP_H
#define __PCP_H

/** @file pcp.h
 *
 *  @brief Pico Color Picker system-wide header file
 */

/* ----------------------------------------------------------------------- */

/* TYPES - to prevent circular includes */

typedef union v32 {
#ifdef __cplusplus
  v32(int32_t x) : s(x) { };
  v32(uint32_t x) : u(x) { };
#endif
  int32_t s;
  uint32_t u;
} v32_t;

typedef struct bitmap bitmap_t;
typedef struct context context_t;

typedef void (*context_callback_f)(context_t *c, void *, v32_t);

/* ----------------------------------------------------------------------- */

/*  Simple object functions -- these may return to whence they came  */

/* ----------------------------------------------------------------------- */

/* MAGIC NUMBERS */

#define TYPE_FILTER         0xFF
#define ASSERT_IS_A(x, y) assert((*((uint32_t *)(x)) & TYPE_FILTER) == ((y) & TYPE_FILTER));

#define UNINITIALIZED         0
#define CONTEXT_SCREEN_T    ( 1 | FREEABLE_P )
#define RGB_ENCODER_T         2
#define RGB_ENCODERS_DATA_T   3
#define CONTEXT_T             4
#define CONTEXT_LEDS_T        5
#define MENU_ITEM_T         ( 6 | FREEABLE_P )
#define MENU_T              ( 7 | FREEABLE_P )
#define CHORD_T               8
#define NOTE_COLOR_T          9
#define RGB_ENCODER_FRAME_T  10  /* Freeable */
#define BITMAP_T             11  /* Freeable */

#define FREEABLE_P          ( 1u << 31 )

/* THREAD_LOCAL_STORAGE (ThLS) */
#define ThLS_BLDR_CTX           0
#define ThLS_BLDR_MENU          1

/* NOTIFICATION INDICES */
#define NTFCN_IDX_EVENT         1
#define NTFCN_IDX_CONTEXT       2
#define NTFCN_IDX_LEDS          2

/* ----------------------------------------------------------------------- */

typedef struct pcp {
  uint32_t magic_number;
  void (*free_f)(void *);
  bool autofree_p;
} pcp_t;

static inline void pcp_free(void *v) {
  pcp_t *p = (pcp_t *)v;
  if (!p->autofree_p || !(p->magic_number & FREEABLE_P)) return;
  (p->free_f ?: vPortFree)(v);   /* GCC has the Elvis operator! */
}

/* ----------------------------------------------------------------------- */

/* CONVENIENT FORMATTING EXPRESSIONS */

#define RE_LABEL_Y_OFFSET (SCREEN_HEIGHT - RE_LABEL_FONT.Height)
#define RE_LABEL_TOTAL_WIDTH (SCREEN_WIDTH - BUTTON_LABEL_FONT.Width)

static inline void *pcp_zero_malloc(size_t s) { return memset(pvPortMalloc(s),0,s); }

#endif /* __PCP_H */

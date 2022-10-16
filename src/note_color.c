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

/** @file note_color.c
 *
 *  @brief Manager the notes to colors mapping, both "permanent" and transient as it
 *         is manipulated through the color menu and through the chord editor.
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "pcp.h"
#include "log.h"

#include "bitmap.h"
#include "button.h"
#include "context.h"
#include "menu.h"
#include "note_color.h"

#define CELL_WIDTH ( RE_LABEL_TOTAL_WIDTH / 3 )

/* ---------------------------------------------------------------------- */

/** @brief Manage the "color" of a particular note in the 12-note space.  */
struct note_color {
    uint32_t magic_number;
    const char *note_name;
    uint32_t rgb;
    context_t *rgbe_ctx;
};

typedef struct {
    uint32_t magic_number;
    bool active;
    uint8_t value;
    uint8_t shift;
    uint8_t button_offset;
} rgb_encoder_t;

typedef struct rgb_encoders_data {
    pcp_t pcp;
    SemaphoreHandle_t rgbe_mutex;
    uint32_t *rgb;
    rgb_encoder_t rgb_encoders[IO_PIO_SLOTS / 2]; /*  We waste storage to simplify lookup.  Maybe not necessary with callbacks?  */
} rgb_encoders_data_t;

typedef struct rgb_encoder_frame {
    pcp_t pcp;
    void ( *line1 )(menu_t *, uint8_t);
    menu_t *menu;
    uint8_t cursor;
} rgb_encoder_frame_t;

typedef struct chord chord_t;
struct chord {
    pcp_t pcp;
    note_color_t *note_colors[3];
    context_leds_t leds;
};

/* ---------------------------------------------------------------------- */

#define NOTE_COUNT 12

static const char *initial_names[NOTE_COUNT] = {
    "C", "C#/Db", "D", "D#/Eb", "E", "F",
    "F#/Gb", "G", "G#/Ab", "A", "A#/B#", "B"
};

static uint32_t initial_rgbs[NOTE_COUNT] = {
    0xFF0000, 0xcc1100, 0xbb2200, 0xcc5500, 0xffcc00, 0x33ff00,
    0x00cd71, 0x008AA1, 0x2161b0, 0x2200ff, 0x860e90, 0xB8154A
};

/* ---------------------------------------------------------------------- */

static note_color_t note_colors[NOTE_COUNT];

static context_leds_t rgbe_leds;

static chord_t chord;

/* ---------------------------------------------------------------------- */

static rgb_encoder_frame_t *s_rgb_encoder_frame_alloc(
        void ( *line1 )(menu_t *, uint8_t), menu_t *menu, uint8_t cursor)
{
    rgb_encoder_frame_t *f = pcp_zero_malloc( sizeof( rgb_encoder_frame_t ) );
    f->pcp.magic_number = RGB_ENCODER_FRAME_T | FREEABLE_P;
    f->pcp.free_f = vPortFree;
    f->pcp.autofree_p = true;
    f->line1 = line1;
    f->menu = menu;
    f->cursor = cursor;
    return f;
}


static uint32_t s_rgbes_value(rgb_encoders_data_t *re)
{
    uint32_t rgb = 0u;
    xSemaphoreTake(re->rgbe_mutex, portMAX_DELAY);
    for (int i = 0; i<4; i++) {
        if (!re->rgb_encoders[i].active)
            continue;
        rgb |= re->rgb_encoders[i].value << re->rgb_encoders[i].shift;
    }
    xSemaphoreGive(re->rgbe_mutex);
    return rgb;
} /* s_rgbes_value */

static void s_rgbes_re_callback(context_t *c, void *re_v, v32_t delta)
{
    rgb_encoder_t *re = (rgb_encoder_t *) re_v;
    assert(re->magic_number == RGB_ENCODER_T);

    log_trace("RGB Encoder initial value %02x", re->value);

    int16_t value = re->value;
    int8_t multiplier = button_depressed_p(re->button_offset) ? 0x1u : 0x11u;
    value += delta.s * multiplier;
    value = MAX(value, 0);
    value = MIN(value, 0xff);
    re->value = value;

    rgb_encoders_data_t *red = (rgb_encoders_data_t *) c->data;
    ASSERT_IS_A(red, RGB_ENCODERS_DATA_T);
    *red->rgb = s_rgbes_value( (rgb_encoders_data_t *) c->data );

    log_trace("RGB Encoder new value %02x", re->value);
}; /* s_rgbes_re_callback */

static void s_rgbe_display_callback(context_t *c, void *data, v32_t v)
{
    static char hex_color_value[8];
    rgb_encoder_frame_t *f = NULL;

    log_trace("Entering RGB Encoder s_rgbe_display_callback");

    if (c == context_current() ) {
        f = (rgb_encoder_frame_t *) context_frame_data();
        if (f)
            ASSERT_IS_A(f, RGB_ENCODER_FRAME_T);
    } else {
        f = NULL;
        log_warn( "Context mismatch: %lx vs %lx", c, context_current() );
    }

    rgb_encoders_data_t *re = ( (rgb_encoders_data_t *) c->data );
    ASSERT_IS_A(re, RGB_ENCODERS_DATA_T);

    log_trace("-- UI Decoder with REs %02x %02x %02x",
            re->rgb_encoders[RE_RED_OFFSET].value,
            re->rgb_encoders[RE_GREEN_OFFSET].value,
            re->rgb_encoders[RE_BLUE_OFFSET].value
            );

    sprintf(hex_color_value, "#%06lx", (unsigned long) *re->rgb);

    if (f)
        f->line1(f->menu, f->cursor);
    bitmap_draw_string(context_get_drawing_pane(c), 0,
            TRIPLE_LINE_TEXT_FONT.Height, &DOUBLE_LINE_TEXT_FONT,
            hex_color_value
            );
} /* s_rgbe_display_callback */

static context_t *s_rgbe_init(uint32_t *rgb)
{
    /*
     *  Step 1 - Initialize the underlying data storage object for the context
     */
    rgb_encoders_data_t *rgbes =
        pcp_zero_malloc( sizeof( rgb_encoders_data_t ) );
    rgbes->pcp.magic_number = RGB_ENCODERS_DATA_T | FREEABLE_P;
    rgbes->pcp.free_f = vPortFree;
    rgbes->pcp.autofree_p = true;
    rgbes->rgbe_mutex = xSemaphoreCreateMutex();
    rgbes->rgb = rgb;

    log_trace("Start context build for RGB Encoder");
    context_builder_init();

    /*
     *  Step 2 - Map encoders to RGB components and initialize value
     */

    rgbes->rgb_encoders[RE_RED_OFFSET].magic_number = RGB_ENCODER_T;
    rgbes->rgb_encoders[RE_RED_OFFSET].active = true;
    rgbes->rgb_encoders[RE_RED_OFFSET].shift = 16;
    rgbes->rgb_encoders[RE_RED_OFFSET].button_offset = BUTTON_RED_OFFSET;
    rgbes->rgb_encoders[RE_RED_OFFSET].value = *rgb >> 16 & 0xff;
    rgbes->rgb_encoders[RE_GREEN_OFFSET].magic_number = RGB_ENCODER_T;
    rgbes->rgb_encoders[RE_GREEN_OFFSET].active = true;
    rgbes->rgb_encoders[RE_GREEN_OFFSET].shift = 8;
    rgbes->rgb_encoders[RE_GREEN_OFFSET].button_offset = BUTTON_GREEN_OFFSET;
    rgbes->rgb_encoders[RE_GREEN_OFFSET].value = *rgb >> 8 & 0xff;
    rgbes->rgb_encoders[RE_BLUE_OFFSET].magic_number = RGB_ENCODER_T;
    rgbes->rgb_encoders[RE_BLUE_OFFSET].active = true;
    rgbes->rgb_encoders[RE_BLUE_OFFSET].shift = 0;
    rgbes->rgb_encoders[RE_BLUE_OFFSET].button_offset = BUTTON_BLUE_OFFSET;
    rgbes->rgb_encoders[RE_BLUE_OFFSET].value = *rgb & 0xff;

    /*
     *  Step 3 - Define context.
     */
    context_builder_set_red_re(s_rgbes_re_callback,
            &rgbes->rgb_encoders[RE_RED_OFFSET], NULL, NULL, "Red"
            );
    context_builder_set_green_re(s_rgbes_re_callback,
            &rgbes->rgb_encoders[RE_GREEN_OFFSET], NULL, NULL, "Green"
            );
    context_builder_set_blue_re(s_rgbes_re_callback,
            &rgbes->rgb_encoders[RE_BLUE_OFFSET], NULL, NULL, "Blue"
            );

    context_builder_set_upper_button(button_return_callback, NULL, LAQUO);

    context_builder_set_display_callback(s_rgbe_display_callback, rgbes);

    context_builder_set_data(rgbes);


    log_trace("Finalizing context build for RGB Encoder");
    return context_builder_finalize();
} /* s_rgbe_init */


static void s_menu_render_item_callback(menu_item_t *item,
        bitmap_t *item_bitmap, uint8_t cursor)
{
    char buffer[25];
    note_color_t *nc = (note_color_t *) menu_item_data(item);
    ASSERT_IS_A(nc, NOTE_COLOR_T);
    assert(cursor == 0);

    bitmap_clear(item_bitmap);
    sprintf(buffer, "%-5s #%06lx", nc->note_name, (unsigned long) nc->rgb);
    bitmap_draw_string(item_bitmap, 8, 0, &TRIPLE_LINE_TEXT_FONT, buffer);
}

static void s_chord_render_item_callback(menu_item_t *item,
        bitmap_t *item_bitmap, uint8_t cursor)
{
    note_color_t *nc = (note_color_t *) menu_item_data(item);
    ASSERT_IS_A(nc, NOTE_COLOR_T);

    bitmap_clear(item_bitmap);
    bitmap_draw_string(item_bitmap,
            ( item_bitmap->width - strlen(nc->note_name) *
              TRIPLE_LINE_TEXT_FONT.Width ) / 2,
            0, &TRIPLE_LINE_TEXT_FONT, nc->note_name
            );
} /* s_chord_render_item_callback */

static void s_menu_line1_render_callback(menu_t *m, uint8_t cursor)
{
    s_menu_render_item_callback(menu_item_at_cursor(m, cursor, 0),
            context_get_drawing_pane(NULL), cursor
            );
}

static void s_chord_line1_render_callback(menu_t *m, uint8_t cursor)
{
    /* TODO: Change the rendering */
    s_menu_render_item_callback(menu_item_at_cursor(m, cursor, 0),
            context_get_drawing_pane(NULL), cursor
            );
}

static void s_menu_selection_changed_callback(menu_t *menu)
{
    rgbe_leds.magic_number = CONTEXT_LEDS_T;
    uint8_t i = menu_cursor_at(menu, 0);
    rgbe_leds.rgb_p[0] = &note_colors[( i + NOTE_COUNT - 1 ) % NOTE_COUNT].rgb;
    rgbe_leds.rgb_p[1] = &note_colors[i].rgb;
    rgbe_leds.rgb_p[2] = &note_colors[( i + 1 ) % NOTE_COUNT].rgb;
}

static void s_chord_selection_changed_callback(menu_t *menu)
{
    rgbe_leds.magic_number = CONTEXT_LEDS_T;
    for (uint8_t i = 0; i<3; i++) {
        rgbe_leds.rgb_p[i] = &note_colors[menu_cursor_at(menu, i)].rgb;
    }
}

static void s_color_menu_entry(context_t *c, void *data, v32_t v)
{
    xTaskNotifyIndexed(tasks.display, NTFCN_IDX_LEDS, (uint32_t) &rgbe_leds,
            eSetValueWithOverwrite
            );
}

/* ---------------------------------------------------------------------- */

void note_color_init()
{
    for (uint8_t i = 0; i<12; i++) {
        note_colors[i].magic_number = NOTE_COLOR_T;
        note_colors[i].note_name = initial_names[i];
        note_colors[i].rgb = initial_rgbs[i];
        log_trace("Initializing RGB Encoder #%d", i);
        note_colors[i].rgbe_ctx = s_rgbe_init(&note_colors[i].rgb);
    }
}

/* ---------------------------------------------------------------------- */

context_t *note_color_menu_alloc()
{
    menu_builder_init(1, NOTE_COUNT);

    for (uint8_t i = 0; i<NOTE_COUNT; i++) {
        menu_builder_set_item_enter_ctx(i, note_colors[i].rgbe_ctx);
        menu_builder_set_item_data(i, &note_colors[i]);
    }

    menu_builder_set_cursor_enter_data(0,
            s_rgb_encoder_frame_alloc(s_menu_line1_render_callback,
                    menu_builder_menu_ptr(), 0
                    )
            );

    menu_builder_set_render_item_cb(s_menu_render_item_callback);
    menu_builder_set_selection_changed_cb(s_menu_selection_changed_callback);
    context_builder_set_enable_callback(s_color_menu_entry, 0);

    return menu_builder_finalize();
} /* note_color_menu_alloc */

context_t *chord_menu_alloc()
{
    menu_builder_init(3, NOTE_COUNT);

    for (uint8_t i = 0; i<NOTE_COUNT; i++) {
        menu_builder_set_item_enter_ctx(i, note_colors[i].rgbe_ctx);
        menu_builder_set_item_data(i, &note_colors[i]);
    }
    for (uint8_t i = 0; i<3; i++) {
        menu_builder_set_cursor_enter_data(0,
                s_rgb_encoder_frame_alloc(s_menu_line1_render_callback,
                        menu_builder_menu_ptr(), i
                        )
                );
    }

    menu_builder_set_render_item_cb(s_chord_render_item_callback);
    menu_builder_set_selection_changed_cb(s_chord_selection_changed_callback);
    context_builder_set_enable_callback(s_color_menu_entry, 0);

    return menu_builder_finalize();
} /* chord_menu_alloc */

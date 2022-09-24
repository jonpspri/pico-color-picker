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

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/sem.h"
#include "pico/multicore.h"

#include "hardware/pio.h"

#include "ws2813b.pio.h"
#include "ws2812.pio.h"

#include "pico_debug.h"

#define WS2813B_PIOx __CONCAT(pio, PICO_WS2813B_PIO)
#define WS2813B_GPIO_FUNC_PIOx __CONCAT(GPIO_FUNC_PIO, PICO_WS2813B_PIO)

#define WS2812_PIOx __CONCAT(pio, PICO_WS2812_PIO)
#define WS2812_GPIO_FUNC_PIOx __CONCAT(GPIO_FUNC_PIO, PICO_WS2812_PIO)

static struct semaphore reset_delay_complete_sem;
static alarm_id_t reset_delay_alarm_id;

static uint32_t grbs[WS2813B_PIXEL_COUNT];
static uint16_t grbs_count;

int64_t reset_delay_complete(alarm_id_t id, void *user_date) {
    reset_delay_alarm_id = 0;
    sem_release(&reset_delay_complete_sem);
    return 0;
}

/*
 * The LED expects GRB (not RGB like the rest of the world.
 */
static inline uint32_t urgb_u32(uint32_t rgb) {
    return ((rgb & 0xff0000) >> 8 ) |
           ((rgb & 0x00ff00) << 8 ) |
           (rgb & 0x0000ff);
}

static inline void ws2813b_put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(WS2813B_PIOx, PICO_WS2813B_SM, pixel_grb << 8u);
}

static inline void ws2812_put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(WS2812_PIOx, PICO_WS2812_SM, pixel_grb << 8u);
}

static void core1_main() {
    int i;
    for (i=0; i<WS2813B_PIXEL_COUNT; i++) ws2813b_put_pixel(grbs[i%grbs_count]);
    for (i=0; i<WS2812_PIXEL_COUNT; i++) ws2812_put_pixel(grbs[i%grbs_count]);
    if (reset_delay_alarm_id) cancel_alarm(reset_delay_alarm_id);
    reset_delay_alarm_id = add_alarm_in_us(500, reset_delay_complete, NULL, true);
}

void ws281x_sparkle_pixels(uint16_t rgb_count, uint32_t *rgbs) {

    sem_acquire_blocking(&reset_delay_complete_sem);
    grbs_count = rgb_count;
    for (int i=0; i<rgb_count; i++) {
        /* debug_printf("Color %d/%d: %06lx", i, rgb_count, rgbs[i]); */
        grbs[i] = urgb_u32(rgbs[i]);
    }
    multicore_reset_core1();
    multicore_launch_core1(core1_main);
}

/*
 * Output WS2813B_PIXEL_COUNT pixels of a given color
 */
void ws281x_pio_init() {
    gpio_set_function(PICO_WS2813B_PIN, WS2813B_GPIO_FUNC_PIOx);
    pio_sm_claim(WS2813B_PIOx, PICO_WS2813B_PIN);
    uint ws2813b_offset = pio_add_program(WS2813B_PIOx, &ws2813b_program);
    ws2813b_program_init(WS2813B_PIOx, PICO_WS2813B_SM, ws2813b_offset, PICO_WS2813B_PIN, false);

    gpio_set_function(PICO_WS2812_PIN, WS2812_GPIO_FUNC_PIOx);
    pio_sm_claim(WS2812_PIOx, PICO_WS2812_PIN);
    uint ws2812_offset = pio_add_program(WS2812_PIOx, &ws2812_program);
    ws2812_program_init(WS2812_PIOx, PICO_WS2812_SM, ws2812_offset, PICO_WS2812_PIN, false);

    sem_init(&reset_delay_complete_sem, 1, 1);
}

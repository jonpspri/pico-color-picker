/*
 * SPDX-FileCopyrightText: 2023 Jonathan Springer
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
#include "hardware/gpio.h"
#include "pico/util/queue.h"

#include "pico_debug.h"
#include "hid.h"

#define PRIOR_A_MASK 0x08
#define PRIOR_B_MASK 0x04
#define CURR_A_MASK 0x02
#define CURR_B_MASK 0x01

typedef struct {
  hid_t hid_type;
  hid_handle hid_handle;
  uint32_t device_data[4];
} hid_generic;

#define ROTARY_ENCODER_MIN 0
#define ROTARY_ENCODER_MAX 0xff
typedef struct rotary_encoder_struct {
  hid_t hid_type;
  hid_handle handle;
  /* Device Data */
  uint32_t pins[2];  // A, B
  uint32_t prior_encoding;   // 0000AB00
  int32_t value;
} rotary_encoder;

typedef struct irq_redirect_entry {
  void (*do_irq)(void *user_data);
  void *user_data;
} irq_redirect_entry_t;

#ifndef MAX_HIDS
#define MAX_HIDS 5
#endif
static uint32_t hid_count;
static hid_generic hids[MAX_HIDS];
static repeating_timer_t hid_timer;

/*
 *  Transition handling code and table
 */
typedef uint8_t transition;
typedef void (* re_operation_transition)(rotary_encoder *re);

static void re_error_transition(rotary_encoder *re) {
  /* debug_printf("%s", "Invalid transition." ); */
}
static void re_null_transition(rotary_encoder *re) { };
static void re_cw_transition(rotary_encoder *re) { re->value = MIN(re->value+1, ROTARY_ENCODER_MAX); }
static void re_ccw_transition(rotary_encoder *re) { re->value = MAX(re->value-1, ROTARY_ENCODER_MIN); }

re_operation_transition transition_table[16] = {
  re_null_transition,  // 0000
  re_ccw_transition,   // 0001
  re_cw_transition,    // 0010
  re_error_transition, // 0011

  re_cw_transition,    // 0100
  re_null_transition,  // 0101
  re_error_transition, // 0110
  re_ccw_transition,   // 0111

  re_ccw_transition,   // 1000
  re_error_transition, // 1001
  re_null_transition,  // 1010
  re_cw_transition,    // 1011

  re_error_transition, // 1100
  re_cw_transition,    // 1101
  re_ccw_transition,   // 1110
  re_null_transition   // 1111
};

static bool hid_process_timer(repeating_timer_t *t) {
  uint32_t values=gpio_get_all();
  for (uint32_t i = 0; i < hid_count; i++) {
    if (hids[i].hid_type != HID_ROTARY_ENCODER) continue;
    rotary_encoder *re = (rotary_encoder *) &hids[i];
    uint32_t index =
      re->prior_encoding
      | ((values >> re->pins[0]) & 1u) << 1u
      | ((values >> re->pins[1]) & 1u);
    (transition_table[index])(re);
    re->prior_encoding = (index & 3u) << 2;
  }
  return true;
}

int32_t hid_encoder_value(hid_handle handle) {
  return ((rotary_encoder *)&hids[handle])->value;
}

void hid_encoder_init(uint pin_a, uint pin_b, hid_handle *handle) {
  assert(hid_count < MAX_HIDS);
  rotary_encoder *re = (rotary_encoder *) &hids[hid_count];
  *handle = hid_count++;

  re->pins[0] = pin_a;
  re->pins[1] = pin_b;
  re->value = 0u;
  re->handle = *handle;

  gpio_set_input_enabled(pin_a, true);
  gpio_set_input_enabled(pin_b, true);
  gpio_pull_up(pin_a);
  gpio_pull_up(pin_b);

  uint32_t gpio_values = gpio_get_all();
  re->prior_encoding = ((gpio_values & (1u<<pin_a)) ? 0x8 : 0) | ((gpio_values & (1u<<pin_b)) ? 0x4 : 0);
}

void hid_start_polling() {
  add_repeating_timer_ms(10, hid_process_timer, NULL, &hid_timer);
}

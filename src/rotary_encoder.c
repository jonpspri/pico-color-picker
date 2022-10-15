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

#include "FreeRTOS.h"
#include "task.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"

#include "context.h"
#include "input.h"
#include "log.h"

#define PIOx __CONCAT(pio, IO_DEVICES_PIO)

#ifdef PCP_TRACK_TRANSITIONS
typedef struct {
  uint32_t rx;
  uint16_t idx;
  int8_t delta;
  int8_t sub_count;
} transition_history_t;
uint8_t transition_history_idx;
#endif

typedef struct {
  bool inverted;
  int8_t sub_count;
  bool enabled;
#ifdef PCP_TRACK_TRANSITIONS
  transition_history_t *transition_history;
#endif
} rotary_encoder_info_t;

static rotary_encoder_info_t rotary_encoders[4];

static const int8_t transitions[16] = {
        0,    // 0 00 -> 00 no movement
        -1,   // 1 00 -> 01 3/4 ccw
        +1,   // 2 00 -> 10 3/4 cw
        0,    // 3 00 -> 11 non-Gray-code transition
        +1,   // 4 01 -> 00 2/4 cw
        0,    // 5 01 -> 01 no movement
        0,    // 6 01 -> 10 non-Gray-code transition
        -1,   // 7 01 -> 11 4/4 ccw
        -1,   // 8 10 -> 00 2/4 ccw
        0,    // 9 10 -> 01 non-Gray-code transition
        0,    // A 10 -> 10 no movement
        +1,   // B 10 -> 11 4/4 cw
        0,    // C 11 -> 00 non-Gray-code transition
        +1,   // D 11 -> 01 1/4 cw
        -1,   // E 11 -> 10 1/4 ccw
        0,    // F 11 -> 11 no movement
};

__isr static void rotary_encoder_interrupt_handler(PIO pio, uint8_t sm, TaskHandle_t task_to_signal) {
  uint32_t notify_bits = 0u;

  while(pio_sm_get_rx_fifo_level(pio, sm)) {
    uint32_t pio_rx = pio_sm_get(pio, sm);

    /*
     * Step 1 - decipher the bits coming from the PIO (Rotary Encoders)
     */
    for(int i=0; i<4; i++) {
      uint16_t rx = pio_rx >> (i*2);
      uint8_t prior_state, new_state;
      rotary_encoder_info_t *re = &rotary_encoders[i];

      if (!re->enabled) continue;

      if (re->inverted) {
        prior_state = (rx & 0x2u)>>1 | (rx & 0x1u)<<1;  /* X...XB'A' -> A'B' */
        new_state = (rx & 0x100u)>>7 | (rx & 0x200u)>>9;    /* BAX..X -> AB */
      } else {
        prior_state = rx & 0x3u;                        /* X..XA'B' -> A'B' */
        new_state = (rx & 0x300u)>>8;                     /* ABX..X -> AB */
      }
      if(new_state == prior_state) continue;

      /*
       * Step 2 - construct an index to the callback table
       */
      uint32_t idx = prior_state<<2 | new_state;

      /*
       * Step 3 - adjust the sub-count (between-detent clicks) and callback if we hit a detent
       */
      re->sub_count += transitions[idx];
      if (re->sub_count >= RE_DIVISOR) {
        /* debug_printf("RE %d +1 (index %d)", i, transition_history_idx); */
        notify_bits |= 1 << (i*2);
        re->sub_count = 0;
      } else if (re->sub_count <= RE_DIVISOR * -1) {
        /* debug_printf("RE %d -1 (index %d)", i, transition_history_idx); */
        notify_bits |= 1 << (i*2+1);
        re->sub_count = 0;
      }
#ifdef PCP_TRACK_TRANSITIONS
      re->transition_history[transition_history_idx].rx = rx;
      re->transition_history[transition_history_idx].idx = idx;
      re->transition_history[transition_history_idx].delta = transitions[idx];
      re->transition_history[transition_history_idx].sub_count = re->sub_count;
      transition_history_idx++;
#endif
    }
  }

  if (notify_bits) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTaskNotifyIndexedFromISR(task_to_signal, 1,
        notify_bits,
        eSetBits,
        &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

static void rotary_encoder_register( uint8_t re_number, bool inverted) {
  rotary_encoders[re_number].inverted = inverted;
  rotary_encoders[re_number].enabled = true;
#ifdef PCP_TRACK_TRANSITIONS
  rotary_encoders[re_number].transition_history =
    memset(pvPortMalloc(sizeof(transition_history_t)*256),0,sizeof(transition_history_t)*256);
#endif
}

static void rotary_encoder_init(uint8_t low_pin, uint8_t sm) {
  log_trace("Initializing encoders on SM %d", sm);
  for(uint8_t re=0; re<4; re++) {
    if(!rotary_encoders[re].enabled) continue;
    for(uint8_t i=0; i<2; i++) input_init_pin(low_pin + re*2 + i);
  }
  input_init_sm(low_pin, sm);
  input_pio_irq_set_handler(PIOx, sm,
      rotary_encoder_interrupt_handler, xTaskGetCurrentTaskHandle(),
      PIO_INTR_SM0_RXNEMPTY_BITS);
}

void rotary_encoder_task(void *parm) {
  context_t *context = NULL;

  rotary_encoder_register(RE_RED_OFFSET, RE_RED_INVERTED);
  rotary_encoder_register(RE_GREEN_OFFSET, RE_GREEN_INVERTED);
  rotary_encoder_register(RE_BLUE_OFFSET, RE_BLUE_INVERTED);
  rotary_encoder_init(RE_LOW_PIN, RE_SM);

  uint32_t bits = 0u;

  for( ;; ) {
    /* Update the context if necessary */
    do {
      xTaskNotifyWaitIndexed(NTFCN_IDX_CONTEXT, 0u, 0u, (uint32_t *)(&context), context? 0 : portMAX_DELAY);
    } while(!context);

    ASSERT_IS_A(context, CONTEXT_T);

    log_trace("Processing Rotary Encoder input");
    for (int i=0; context && i<4; i++, bits >>= 2) {
      context_callback_t *c = &context->re_ccb[i];
      if(!c->callback || !(bits & 3u)) continue;
      v32_t delta = (v32_t)((int32_t)0);
      if(bits & 1u) delta.s=1;
      if(bits & 2u) delta.s=-1;
      log_trace("Sending delta %d to RE %d", delta.s, i);
      c->callback(context, c->data, delta);
    }

    log_trace("Rotary encoder->UI Notification");
    context_notify_display_task(context);

    /* Spin-wait for the next event */
    while (!xTaskNotifyWaitIndexed(NTFCN_IDX_EVENT, 0u, 0xFFFFFFFFu, &bits, portMAX_DELAY));
  }
}

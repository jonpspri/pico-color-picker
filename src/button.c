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

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

#include "hardware/pio.h"

/* pico-color-picker includes */
#include "button.h"
#include "context.h"
#include "input.h"
#include "log.h"

#define PIOx __CONCAT(pio, IO_DEVICES_PIO)

/*-----------------------------------------------------------*/

/*  So far, we're just tracking the state of the buttons */

uint8_t buttons;
uint8_t buttons_depressed;

static inline void button_register_button(uint8_t index) { buttons |= 1<<index; }

static __isr void button_irq_handler(PIO pio, uint8_t sm, TaskHandle_t task_to_signal) {
  uint32_t notify_bits = 0u;
  while(pio_sm_get_rx_fifo_level(pio, sm)) {
    uint32_t pio_data = pio_sm_get(pio, sm);

    for(int i=0; i<8; i++) {
      if (!(buttons&(1<<i))) continue;

      uint8_t prior_state, new_state;
      prior_state = (pio_data >> i) & 0x1u;
      new_state = (pio_data >> (8+i)) & 0x1u;

      if(new_state ^ prior_state) {
        uint32_t notify_temp = 1u;
        notify_temp |= new_state ? 0 : 2u;
        notify_bits |= notify_temp << (i*2);
      }
    }
  }

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xTaskNotifyIndexedFromISR(task_to_signal, 1,
      notify_bits,
      eSetBits,
      &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void button_task(void *parm) {
  uint32_t bits = 0u;

  button_register_button(BUTTON_UPPER_OFFSET);
  button_register_button(BUTTON_LOWER_OFFSET);
  button_register_button(BUTTON_RED_OFFSET);
  button_register_button(BUTTON_GREEN_OFFSET);
  button_register_button(BUTTON_BLUE_OFFSET);
  button_init(BUTTON_LOW_PIN, BUTTON_SM);

  for( ;; ) {
    /* Update the callbacks list if necessary */
    context_t *context = NULL;
    xTaskNotifyWaitIndexed(NTFCN_IDX_CONTEXT, 0u, 0u, (uint32_t *)&context, 0);
    assert(!context || context->magic_number == CONTEXT_T);

    for (int i=0; context && i<8 && bits; i++, bits >>= 2) {
      if(!(bits & 1u)) continue;
      if (bits & 2u) {
        buttons_depressed |= 1<<i;
      } else {
        buttons_depressed &= ~(1<<i);
      }
      context_callback_t *c = &context->callbacks->button[i];
      if(c->callback) c->callback(context, c->data, (v32_t)(bits & 2u));
    }

    if (context->callbacks->screen.callback) {
      context->callbacks->screen.callback(context, context->callbacks->screen.data, (v32_t)0ul);
    }

    if (!xTaskNotifyWaitIndexed(1, 0u, 0xFFFFFFFFu, &bits, portMAX_DELAY)) continue;
  }
}

void button_return_callback(context_t *c, void *data, v32_t value) {
  assert(c->parent);
  if (value.u) context_enable(c->parent);
}

void button_init(uint8_t low_pin, uint8_t sm) {
  log_trace("Initializing buttons on SM %d", sm);
  for(uint8_t b=0; b<8; b++) {
    if(buttons&(1<<b)) input_init_pin(low_pin + b);
  }
  input_init_sm(low_pin, sm);
  input_pio_irq_set_handler(PIOx, sm,
      button_irq_handler, xTaskGetCurrentTaskHandle(),
      PIO_INTR_SM0_RXNEMPTY_BITS);
}

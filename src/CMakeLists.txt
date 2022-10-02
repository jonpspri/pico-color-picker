# SPDX-FileCopyrightText: 2022 Jonathan Springer
#
# SPDX-License-Identifier: GPL-3.0-or-later

# This file is part of pico-color-picker.
#
# pico-color-picker is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# pico-color-picker is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# pico-color-picker. If not, see <https://www.gnu.org/licenses/>.

# --------------------------------------------------------------------------------

#
#  Compile-time configuration of the fonts to be used.
#
set(SINGLE_LINE_TEXT_FONT spleen_12x24)
set(TRIPLE_LINE_TEXT_FONT spleen_5x8)
set(RE_LABEL_FONT spleen_5x8)
set(BUTTON_LABEL_FONT Dina_r400_8)
set(FONTS
  ${SINGLE_LINE_TEXT_FONT}
  ${TRIPLE_LINE_TEXT_FONT}
  ${RE_LABEL_FONT}
  ${BUTTON_LABEL_FONT})
list(REMOVE_DUPLICATES FONTS)
message(STATUS "Fonts: ${FONTS}")

set(FONT_SOURCES "${FONTS}")
list(TRANSFORM FONT_SOURCES PREPEND "fonts/")
list(TRANSFORM FONT_SOURCES APPEND ".c")
message(STATUS "Font sources: ${FONT_SOURCES}")

set(FONT_XMACROS_LIST "${FONTS}")
list(TRANSFORM FONT_XMACROS_LIST PREPEND "X(")
list(TRANSFORM FONT_XMACROS_LIST APPEND ")")
string(REPLACE ";" " " FONT_XMACROS "${FONT_XMACROS_LIST}")
message(STATUS "Font X Macros: ${FONT_XMACROS}")

set(FONT_DEFINES
  PICKER_FONTS=${FONT_XMACROS}
  SINGLE_LINE_TEXT_FONT=${SINGLE_LINE_TEXT_FONT}
  TRIPLE_LINE_TEXT_FONT=${TRIPLE_LINE_TEXT_FONT}
  RE_LABEL_FONT=${RE_LABEL_FONT}
  BUTTON_LABEL_FONT=${BUTTON_LABEL_FONT})

# --------------------------------------------------------------------------------

add_executable(pico_color_picker
  bitmap.cpp
  buttons.cpp
  context.cpp
  io_devices.c
  log.c
  main.cpp
  rgb_encoders.cpp
  ssd1306.c
  ws281x.c

  ${FONT_SOURCES}
)
set_property(TARGET pico_color_picker PROPERTY C_STANDARD 11)

pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/ws2813b.pio)
pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/io_devices.pio)

pico_enable_stdio_usb(pico_color_picker 1)
pico_enable_stdio_uart(pico_color_picker 0)

target_compile_definitions(pico_color_picker PRIVATE
  ${FONT_DEFINES}

  PICO_WS2812_PIO=0
  PICO_WS2813B_PIO=0
  PICO_WS2812_SM=0
  PICO_WS2813B_SM=1
  PICO_WS2812_PIN=2
  PICO_WS2813B_PIN=3
  WS2812_PIXEL_COUNT=3
  WS2813B_PIXEL_COUNT=14

  IO_DEVICES_PIO=1

  ROTARY_ENCODER_SM=0
  ROTARY_ENCODER_DIVISOR=4
  ROTARY_ENCODER_LOW_PIN=14
  ROTARY_ENCODER_RED_OFFSET=0
  ROTARY_ENCODER_RED_INVERTED=false
  ROTARY_ENCODER_GREEN_OFFSET=1
  ROTARY_ENCODER_GREEN_INVERTED=true
  ROTARY_ENCODER_BLUE_OFFSET=3
  ROTARY_ENCODER_BLUE_INVERTED=false

  BUTTON_SM=1
  BUTTON_LOW_PIN=6
  BUTTON_UPPER_OFFSET=0
  BUTTON_LOWER_OFFSET=1
  BUTTON_RED_OFFSET=7
  BUTTON_GREEN_OFFSET=6
  BUTTON_BLUE_OFFSET=5

  SCREEN_I2C=i2c1
  SCREEN_SDA_PIN=18
  SCREEN_SCL_PIN=19
  SCREEN_I2C_ADDRESS=0x3C
  SCREEN_WIDTH=128
  SCREEN_HEIGHT=32

  LOG_USE_COLOR
  LOG_LEVEL=LOG_DEBUG
  PICO_DEBUG_MALLOC
  )

target_include_directories(pico_color_picker PUBLIC ${CMAKE_SOURCE_DIR}/config)

target_link_libraries(pico_color_picker PRIVATE
  FreeRTOS-Kernel
  FreeRTOS-Kernel-Heap3
  pico_stdlib
  pico_binary_info
  pico_mem_ops
  pico_multicore
  pico_time
  hardware_i2c
  hardware_pio
  )

pico_add_extra_outputs(pico_color_picker)

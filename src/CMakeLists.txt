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
#  The linker is pretty good at weeding out dead modules,
#  so I'm not sweating including all the code in this
#  list.
#
add_executable(pico_color_picker
  main.c
  ws281x.c
  ssd1306.c
  pico_ft2.c
  pio_irq.c
  rotary_encoder.c
)

pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/ws2813b.pio)
pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/rotary_encoder.pio)

pico_enable_stdio_usb(pico_color_picker 1)
pico_enable_stdio_uart(pico_color_picker 0)

target_compile_definitions(pico_color_picker PRIVATE
  PICO_WS2812_PIO=0
  PICO_WS2813B_PIO=0
  PICO_WS2812_SM=0
  PICO_WS2813B_SM=1
  PICO_WS2812_PIN=2
  PICO_WS2813B_PIN=3
  WS2812_PIXEL_COUNT=3
  WS2813B_PIXEL_COUNT=14
  ROTARY_ENCODER_PIO=1
  ROTARY_ENCODER_DIVISOR=4
  ROTARY_ENCODER_RED_LOW_PIN=14
  ROTARY_ENCODER_RED_INVERTED=false
  ROTARY_ENCODER_GREEN_LOW_PIN=16
  ROTARY_ENCODER_GREEN_INVERTED=true
  ROTARY_ENCODER_BLUE_LOW_PIN=20
  ROTARY_ENCODER_BLUE_INVERTED=false
  BUTTON1_PIN=6
  BUTTON2_PIN=7
  SCREEN_I2C=i2c1
  SCREEN_SDA_PIN=18
  SCREEN_SCL_PIN=19
  SCREEN_I2C_ADDRESS=0x3C
  SCREEN_WIDTH=128
  SCREEN_HEIGHT=32
  )

target_include_directories(pico_color_picker PUBLIC
  ${CMAKE_SOURCE_DIR}/freetype/include
  )

target_link_libraries(pico_color_picker PRIVATE
  pico_stdlib
  pico_binary_info
  pico_multicore
  pico_time
  hardware_i2c
  hardware_pio
  freetype
  ibm_plex_mono
  )

pico_add_extra_outputs(pico_color_picker)

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
)

pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/ws2813b.pio)

pico_enable_stdio_usb(pico_color_picker 1)
pico_enable_stdio_uart(pico_color_picker 0)

target_compile_definitions(pico_color_picker PRIVATE
  DEBUG=0
  )

#    target_compile_options(pico_color_picker PRIVATE -Werror)
target_link_libraries(pico_color_picker PRIVATE
  pico_stdlib
  pico_binary_info
  hardware_pio
  )

pico_add_extra_outputs(pico_color_picker)

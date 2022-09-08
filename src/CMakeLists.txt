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
)

pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)
pico_generate_pio_header(pico_color_picker
  ${CMAKE_CURRENT_LIST_DIR}/ws2813b.pio)

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
  DEBUG=1
  )

target_include_directories(pico_color_picker PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/freetype/include
  )

#    target_compile_options(pico_color_picker PRIVATE -Werror)
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

add_library(freetype OBJECT
  freetype/src/autofit/autofit.c
  freetype/src/base/ftbase.c
  freetype/src/base/ftbbox.c
  freetype/src/base/ftbdf.c
  freetype/src/base/ftbitmap.c
  freetype/src/base/ftcid.c
  freetype/src/base/ftfstype.c
  freetype/src/base/ftgasp.c
  freetype/src/base/ftglyph.c
  freetype/src/base/ftgxval.c
  freetype/src/base/ftinit.c
  freetype/src/base/ftmm.c
  freetype/src/base/ftotval.c
  freetype/src/base/ftpatent.c
  freetype/src/base/ftpfr.c
  freetype/src/base/ftstroke.c
  freetype/src/base/ftsynth.c
  freetype/src/base/fttype1.c
  freetype/src/base/ftwinfnt.c
  freetype/src/bdf/bdf.c
  freetype/src/bzip2/ftbzip2.c
  freetype/src/cache/ftcache.c
  freetype/src/cff/cff.c
  freetype/src/cid/type1cid.c
  freetype/src/gzip/ftgzip.c
  freetype/src/lzw/ftlzw.c
  freetype/src/pcf/pcf.c
  freetype/src/pfr/pfr.c
  freetype/src/psaux/psaux.c
  freetype/src/pshinter/pshinter.c
  freetype/src/psnames/psnames.c
  freetype/src/raster/raster.c
  freetype/src/sdf/sdf.c
  freetype/src/sfnt/sfnt.c
  freetype/src/smooth/smooth.c
  freetype/src/svg/svg.c
  freetype/src/truetype/truetype.c
  freetype/src/type1/type1.c
  freetype/src/type42/type42.c
  freetype/src/winfonts/winfnt.c
  freetype/src/base/ftsystem.c
  freetype/src/base/ftdebug.c
)

target_include_directories(freetype PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/freetype/include
  )

target_compile_definitions(freetype PRIVATE
  FT2_BUILD_LIBRARY=1
  FT_CONFIG_OPTION_ERROR_STRINGS=1
  )

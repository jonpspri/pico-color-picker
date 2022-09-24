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
# You should have received a copy of the GNU General Public License along with# pico-color-picker. If not, see <https://www.gnu.org/licenses/>.

#
#  List built based on directions in freetype/docs/INSTALL.ANY
add_library(freetype OBJECT
  freetype/src/base/ftsystem.c
  freetype/src/base/ftinit.c
  freetype/src/base/ftdebug.c
  freetype/src/base/ftbase.c
  freetype/src/base/ftbbox.c
  freetype/src/base/ftglyph.c
  freetype/src/base/ftbitmap.c
  freetype/src/cff/cff.c
  freetype/src/sfnt/sfnt.c
  freetype/src/pshinter/pshinter.c
  freetype/src/psnames/psnames.c
  freetype/src/psaux/psaux.c
  freetype/src/raster/raster.c

  # freetype/src/autofit/autofit.c
  # freetype/src/base/ftcid.c
  # freetype/src/base/ftfstype.c
  # freetype/src/base/ftgasp.c
  # freetype/src/base/ftgxval.c
  # freetype/src/base/ftmm.c
  # freetype/src/base/ftotval.c
  # freetype/src/base/ftpatent.c
  # freetype/src/base/ftpfr.c
  # freetype/src/base/ftstroke.c
  # freetype/src/base/ftsynth.c
  # freetype/src/base/fttype1.c
  # freetype/src/base/ftwinfnt.c
  # freetype/src/bdf/bdf.c
  # freetype/src/bzip2/ftbzip2.c
  # freetype/src/cache/ftcache.c
  # freetype/src/cff/cff.c
  # freetype/src/cid/type1cid.c
  # freetype/src/gzip/ftgzip.c
  # freetype/src/lzw/ftlzw.c
  # freetype/src/pcf/pcf.c
  # freetype/src/pfr/pfr.c
  # freetype/src/sdf/sdf.c
  # freetype/src/smooth/smooth.c
  # freetype/src/svg/svg.c
  # freetype/src/truetype/truetype.c
  # freetype/src/type1/type1.c
  # freetype/src/type42/type42.c
  # freetype/src/winfonts/winfnt.c
)

target_include_directories(freetype BEFORE PUBLIC
  ${CMAKE_SOURCE_DIR}/config
  ${CMAKE_SOURCE_DIR}/freetype/include
  )

target_compile_definitions(freetype PRIVATE
  FT2_BUILD_LIBRARY=1
  )

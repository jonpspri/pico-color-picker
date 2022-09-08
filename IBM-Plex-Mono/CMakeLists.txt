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


set(FONT_OBJ_FILES "")
set(FONT_OTF_FILES
  IBMPlexMono-BoldItalic.otf
  IBMPlexMono-Bold.otf
  IBMPlexMono-ExtraLightItalic.otf
  IBMPlexMono-ExtraLight.otf
  IBMPlexMono-Italic.otf
  IBMPlexMono-LightItalic.otf
  IBMPlexMono-Light.otf
  IBMPlexMono-MediumItalic.otf
  IBMPlexMono-Medium.otf
  IBMPlexMono-Regular.otf
  IBMPlexMono-SemiBoldItalic.otf
  IBMPlexMono-SemiBold.otf
  IBMPlexMono-TextItalic.otf
  IBMPlexMono-Text.otf
  IBMPlexMono-ThinItalic.otf
  IBMPlexMono-Thin.otf
  )
foreach(FILENAME ${FONT_OTF_FILES})
  add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.obj
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_OBJCOPY}
    ARGS -I binary -O arm-none-eabi
    --rename-section .data=.flashdata_$<MAKE_C_IDENTIFIER:${FILENAME}>
      ${FILENAME} ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.obj
      )
  list(APPEND FONT_OBJ_FILES ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.obj)
endforeach(FILENAME)

add_library(ibm_plex_mono STATIC ${FONT_OBJ_FILES})
set_target_properties(ibm_plex_mono PROPERTIES LINKER_LANGUAGE C)

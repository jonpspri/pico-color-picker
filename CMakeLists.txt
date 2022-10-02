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

cmake_minimum_required(VERSION 3.15)

#
#  Include the PICO SDK (out of tree) and FreeRTOS kernal (submodule)
#
include(${CMAKE_SOURCE_DIR}/pico_sdk_import.cmake)
include(${CMAKE_SOURCE_DIR}/pico_extras_import.cmake)
set(FREERTOS_KERNEL_PATH ${CMAKE_SOURCE_DIR}/FreeRTOS-Kernel)
include(${CMAKE_SOURCE_DIR}/FreeRTOS_Kernel_import.cmake)
include(ExternalProject)

project(pico_piano C CXX ASM)
pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)

#  Some gdb customization to be used from within the build directory
file(COPY .gdbinit DESTINATION .)

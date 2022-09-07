# SPDX-FileCopyrightText: 2022 Jonathan Springer
#
# SPDX-License-Identifier: GPL-3.0-or-later

# This file is part of ColorPicker.
#
# ColorPicker is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# ColorPicker is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# ColorPicker. If not, see <https://www.gnu.org/licenses/>.

"""CircuitPython Prototype for the CircuitPicker Board."""
import time
import board
import neopixel
import busio
import rotaryio
import displayio
import digitalio
import terminalio
import adafruit_displayio_ssd1306
from adafruit_display_text import label
from adafruit_bitmap_font import bitmap_font

pixel_pin = board.GP3
num_pixels = 3

pixels = neopixel.NeoPixel(pixel_pin, num_pixels, auto_write=False)

i2c = busio.I2C(board.GP17, board.GP16);
display_bus = displayio.I2CDisplay(i2c, device_address=0x3c)
display = adafruit_displayio_ssd1306.SSD1306(display_bus, width=128, height=32)
font = bitmap_font.load_font("ibmplexmono.bdf", displayio.Bitmap)

red_encoder = rotaryio.IncrementalEncoder(board.GP14, board.GP15)
green_encoder = rotaryio.IncrementalEncoder(board.GP12, board.GP13)
blue_encoder = rotaryio.IncrementalEncoder(board.GP10, board.GP11)

last_position = (red_encoder.position, green_encoder.position, blue_encoder.position)
rgb = (0,0,0)
pixels.fill(rgb)
pixels.show()

btn = digitalio.DigitalInOut(board.GP2)
btn.direction = digitalio.Direction.INPUT
btn.pull = digitalio.Pull.UP

def minmaxdiff(value, diff, mul):
    return max(min(value+diff*mul, 255),0)

def tuple_as_hex(rgb_tuple):
    return ''.join('{:02X}'.format(a) for a in rgb_tuple)

while True:

    position = (red_encoder.position, green_encoder.position, blue_encoder.position)

    if position != last_position:
        difference = (position[0] - last_position[0], position[1] - last_position[1], position[2] - last_position[2])
        mul = 17
        if not btn.value:
            mul = 1
        rgb =(minmaxdiff(rgb[0], difference[0], mul),
              minmaxdiff(rgb[1], difference[1], mul),
              minmaxdiff(rgb[2], difference[2], mul) )
        rgb_hex = tuple_as_hex(rgb)
        print(position, difference, rgb_hex)
        last_position = position
        pixels.fill(rgb)
        pixels.show()
        text_area = label.Label(font, text=rgb_hex)
        text_area.y = 16
        text_area.x = 6
        display.show(text_area)

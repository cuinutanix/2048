#!/usr/bin/env python
#
# Copyright (c) 2014 Nutanix, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Converts a BDF bitmap font to C header file so it can be embedded into C
# programs.
#
# Usage:
#
#   bdf_to_c.py < in.bdf > out.h
#

import sys

fontmap = {}
cur_char = None
in_bitmap = False
font_width = None
font_height = None
for line in sys.stdin:
    parts = line.split()
    if cur_char is None and parts[0] == "FONTBOUNDINGBOX":
      font_width = int(parts[1])
      font_height = int(parts[2])
      if font_width <= 8:
        font_bit_width = 8
      elif font_width <= 16:
        font_bit_width = 16
      elif font_width <= 32:
        font_bit_width = 32
      else:
        assert False, "Font too wide"

    if cur_char is None and parts[0] == "ENCODING":
        cur_char = []
        fontmap[int(parts[1])] = cur_char
    elif cur_char is not None and parts[0] == "BITMAP":
        in_bitmap = True
    elif in_bitmap and parts[0] == "ENDCHAR":
        cur_char = None
        in_bitmap = False
    elif in_bitmap:
        cur_char.append(int(parts[0], 16))

sys.stdout.write("#define FONT_X_RES %d\n" % font_width)
sys.stdout.write("#define FONT_Y_RES %d\n" % font_height)
sys.stdout.write("static const u%d FONT_BITMAP[][%d] = {\n" %
                 (font_bit_width, font_height))

def bit_flip(x):
  r = 0
  for _ in xrange(font_bit_width):
    r = (r << 1) | (x & 1)
    x >>= 1
  return r

for x in xrange(128):
    bitmap = fontmap.get(x, [])
    sys.stdout.write("  /* code point %x */\n" % x)
    sys.stdout.write("  {\n")
    for row in bitmap:
        sys.stdout.write("    0x%x,\n" % bit_flip(row))
    sys.stdout.write("  },\n")

sys.stdout.write("};\n")

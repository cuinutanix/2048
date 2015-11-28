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

#
# Fixes up option ROM to pad the size and fill in checksum.
#
# Usage:
#  python fix_rom.py < in.bin > out.rom
#

from cStringIO import StringIO

import sys
import shutil
import struct

rom_data = StringIO()
shutil.copyfileobj(open(sys.argv[1]), rom_data)

# Pad size
pad_size = 512 - (rom_data.tell() % 512)
rom_data.write("\x00" * pad_size)
rom_size = rom_data.tell()
rom_data.seek(2)
rom_data.write(struct.pack("B", rom_size / 512))

# Do checksum
rom_data.seek(0)
checksum = 0
while True:
  buf = rom_data.read(8)
  if not buf: break
  checksum += sum(struct.unpack("BBBBBBBB", buf))
rom_data.seek(0x25) # offset of _pnph_checksum
rom_data.write(struct.pack("B", 256 - (checksum % 256)))

# Verify checksum
checksum = 0
rom_data.seek(0)
while True:
  buf = rom_data.read(8)
  if not buf: break
  checksum += sum(struct.unpack("BBBBBBBB", buf))
assert checksum % 256 == 0

rom_data.seek(0)
shutil.copyfileobj(rom_data, open(sys.argv[2], "w"))

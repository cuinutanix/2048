/*
 * Copyright (c) 2014 Nutanix Inc. All rights reserved.
 *
 * Author: cui@nutanix.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "image.h"
#include "fb.h"

void display_image(int row, int col, const void *image)
{
  const u16 *p = image;
  u32 width  = *p++;
  u32 height = *p++;
  u32 pos;

  u16 palette[128];

  for (pos = 0; pos < width * height; ) {
    u16 word = *p++;
    u8 index = word >> 8;

    if ((index & 0x80)) {
      palette[index & 0x7f] = rgb24to16((word << 16) | *p++);
    } else {
      u8 rl = word;
      u16 color = palette[index];

      do {
        set_pixel(row + pos / width, col + pos % width, color);
        pos++;
      } while (rl-- > 0);
    }
  }
}

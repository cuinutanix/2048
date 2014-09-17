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

#include "console.h"
#include "inout.h"

typedef struct {
  int row;
  int col;
  u8 color;
  int width;
  int height;
  draw_char_fn draw_char;
} console_state_t;

static console_state_t state;

#define RGB(r, g, b) ((r << 16) | (g << 8) | b)
const u32 VGA_PALETTE[16] = {
  RGB(12,77,162),     // "BLACK"
  RGB(201,192,121),
  RGB(168,160,100),
  RGB(169,134,96),

  RGB(169,107,93),
  RGB(177,94,83),
  RGB(136,75,67),
  RGB(230,236,237),  // LGREY

  RGB(196,208,213),  // DGREY
  RGB(158,58,62),
  RGB(158,185,94),
  RGB(202,198,90),

  RGB(249,186,79),
  RGB(255,237,81),
  RGB(135,105,60),
  RGB(255,255,255),  // WHITE
};


void set_color(u8 color)
{
  state.color = color;
}

void set_cursor(int row, int col)
{
  state.row = row;
  state.col = col;
}

static void draw_char(char c, u8 color, int row, int col)
{
  struct {
    char ch;
    u8 color;
  } *const CONSOLE = (void *)0xB8000;
  CONSOLE[row*80 + col].ch = c;
  CONSOLE[row*80 + col].color = color;
}

void putc(char c)
{
  if (c == '\n') {
    while (state.col < state.width) {
      state.draw_char(' ', state.color, state.row, state.col++);
    }
  } else if (c == '\t') {
    const int next_col = (state.col & -8) + 8;
    while (state.col < next_col && state.col < state.width) {
      state.draw_char(' ', state.color, state.row, state.col++);
    }
  } else {
    state.draw_char(c, state.color, state.row, state.col);
    state.col++;
  }

  if (state.col == state.width) {
    state.col = 0;
    state.row += 1;
  }

  if (state.row == state.height) {
    state.row = 0;
  }
}

void print(const char *str)
{
  while(*str) {
    putc(*str++);
  }
}

static inline u8 to6bit(u8 color)
{
  return color >> 2;
}

void console_init()
{
  int i;

  state.row = state.col = 0;
  state.draw_char = draw_char;
  state.color = 0x07;
  state.width = 80;
  state.height = 25;

  outb(0x3c6, 0xff);
  for (i = 0; i < 16; i++) {
    u32 color = VGA_PALETTE[i];

    outb(0x3c8, i);
    outb(0x3c9, to6bit(color >> 16));
    outb(0x3c9, to6bit(color >> 8));
    outb(0x3c9, to6bit(color));
  }

  for (i = 0; i < 80*25; i++) {
    putc(' ');
  }

  // Toss away the cursor.
  outb(0x3d4, 0xe);
  outb(0x3d5, 0xff);
  outb(0x3d4, 0xf);
  outb(0x3d5, 0xff);
}

void console_upgrade(int width, int height, draw_char_fn fn)
{
  state.row = state.col = 0;
  state.draw_char = fn;
  state.width = width;
  state.height = height;
}

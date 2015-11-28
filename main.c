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

#include "types.h"

#include "pic.h"
#include "pit.h"
#include "console.h"
#include "fb.h"
#include "2048.h"
#include "image.h"

#ifdef HAS_NUTANIX_LOGO
#include "assets/login_logo.h"
#endif

void timer_interrupt()
{
  static u32 count = 0;

  if (count++ % 10 == 0) {
    game_tick();
  }

  PIC_ack(0);
}

void keyboard_interrupt()
{
  int scan = inb(0x60);
  int cur_move = -1;
  int reset = 0;
  int session;

  static int ctrl = 0;
  static int alt  = 0;

  if (scan == 0xe0) {
    scan = inb(0x60);
  }

  switch(scan & 0x7f) {
    case 0x1:
      reset = 1;
      break;
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
      session = (scan & 0x7f) - 2;
      if (session < 0) {
        session = 0;
      }
      if (ctrl) {
        game_save(session);
      } else {
        game_restore(session);
      }
      break;
    case 0x1d:
      ctrl = (scan & 0x80) == 0;
      break;
    case 0x38:
      alt = (scan & 0x80) == 0;
      break;
    case 0x48:
      cur_move = UP;
      break;
    case 0x4b:
      cur_move = LEFT;
      break;
    case 0x4d:
      cur_move = RIGHT;
      break;
    case 0x50:
      cur_move = DOWN;
      break;
    case 0x53:
      if (ctrl && alt) {
        outb(0x64, 0xfe); // Really!?
      }
      break;
  }

  if ((scan & 0x80) == 0) {
    if (cur_move != -1) {
      game_input(cur_move);
    } else if (reset) {
      game_reset();
    }
  }

  PIC_ack(1);
}

int main()
{
  console_init();
  PIC_init();
  timer_init((u16)(PIT_FREQUENCY_HZ/100 + 0.5));

  PIC_unmask(0);
  PIC_unmask(1);

  fb_init();

  set_color(0x0f);
  extern const char ERROR_MESSAGE[];
  print(ERROR_MESSAGE);
  print(" Please install an operating system, or play 2048.");

#ifdef HAS_NUTANIX_LOGO
  display_image(500, 227, login_logo_image);
#endif

  set_cursor(29, 0);
  print("2048 was created by Gabriele Cirulli. "
	"http://gabrielecirulli.github.io/2048/");

  game_reset();

  asm volatile("sti");
  while(1) {
    asm volatile("hlt");
  }

  return 0;
}

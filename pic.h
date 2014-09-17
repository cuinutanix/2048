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

#ifndef __PIC_H__
#define __PIC_H__

#define PIC1 0x20
#define PIC2 0xA0

#define PIC1_COMMAND PIC1
#define PIC2_COMMAND PIC2

#define PIC1_DATA (PIC1+1)
#define PIC2_DATA (PIC2+1)

#define PIC_EOI 0x20

#define ICW1_ICW4        0x01                /* ICW4 (not) needed */
#define ICW1_SINGLE      0x02                /* Single (cascade) mode */
#define ICW1_INTERVAL4   0x04                /* Call address interval 4 (8) */
#define ICW1_LEVEL       0x08                /* Level triggered (edge) mode */
#define ICW1_INIT        0x10                /* Initialization - required! */

#define ICW4_8086        0x01                /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO        0x02                /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE   0x08                /* Buffered mode/slave */
#define ICW4_BUF_MASTER  0x0C                /* Buffered mode/master */
#define ICW4_SFNM        0x10                /* Special fully nested (not) */

#include "inout.h"

static inline void PIC_init()
{
  outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
  io_wait();
  outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
  io_wait();
  outb(PIC1_DATA, 0x30);                 // ICW2: Master PIC vector offset
  io_wait();
  outb(PIC2_DATA, 0x38);                 // ICW2: Slave PIC vector offset
  io_wait();
  outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
  io_wait();
  outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
  io_wait();

  outb(PIC1_DATA, ICW4_8086);
  io_wait();
  outb(PIC2_DATA, ICW4_8086);
  io_wait();

  outb(PIC1_DATA, 0xff);
  outb(PIC2_DATA, 0xff);
}

static inline void PIC_ack(int vec)
{
  if ((vec & 8)) {
    outb(PIC2_COMMAND, PIC_EOI);
  }
  outb(PIC1_COMMAND, PIC_EOI);
}

static inline void PIC_unmask(int vec)
{
  u16 port = (vec & 8) ? PIC2_DATA : PIC1_DATA;
  u8 mask;

  vec &= 7;

  mask = inb(port);
  mask &= ~(1 << vec);
  outb(port, mask);
}

static inline void PIC_mask(int vec)
{
  u16 port = (vec & 8) ? PIC2_DATA : PIC1_DATA;
  u8 mask;

  vec &= 7;

  mask = inb(port);
  mask |= (1 << vec);
  outb(port, mask);
}

#endif

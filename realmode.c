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

// GNU assemlber hack For executing 32-bit code in real mode. Even though
// The code is 32-bit for data operations, pointers are still only 16-bit wide
// so the total addressable memory is the first 64K. Do the minimal for
// setting up protected mode and nothing fancy.

asm(".code16gcc\n");

#include "dt.h"

gdt_entry GDT[4] = {};
idt_entry IDT[256] = {};

dtr IDTR = {};
dtr GDTR = {};

u32 SEGMENT_BASE_ADDRESS = 0;

char VBE_INFO[512] = {};
char VBE_MODE_INFO[256] = {};
u8   VBE_SUCCESS = 0;

extern void irq0();
extern void irq1();

void setup_descriptors()
{
  GDT[1].seg_limit_lo = 0xffff;
  GDT[1].seg_limit_hi = 0xf;
  GDT[1].base_lo = SEGMENT_BASE_ADDRESS;
  GDT[1].base_mid = SEGMENT_BASE_ADDRESS >> 16;
  GDT[1].base_hi = SEGMENT_BASE_ADDRESS >> 24;
  GDT[1].type = SEG_TYPE_CODE;
  GDT[1].s = 1;
  GDT[1].p = 1;
  GDT[1].d = 0;
  GDT[1].g = 1;

  GDT[2].seg_limit_lo = 0xffff;
  GDT[2].seg_limit_hi = 0xf;
  GDT[2].base_lo = SEGMENT_BASE_ADDRESS;
  GDT[2].base_mid = SEGMENT_BASE_ADDRESS >> 16;
  GDT[2].base_hi = SEGMENT_BASE_ADDRESS >> 24;
  GDT[2].type = SEG_TYPE_CODE;
  GDT[2].s = 1;
  GDT[2].p = 1;
  GDT[2].d = 1;
  GDT[2].g = 1;

  GDT[3].seg_limit_lo = 0xffff;
  GDT[3].seg_limit_hi = 0xf;
  GDT[3].base_lo = SEGMENT_BASE_ADDRESS;
  GDT[3].base_mid = SEGMENT_BASE_ADDRESS >> 16;
  GDT[3].base_hi = SEGMENT_BASE_ADDRESS >> 24;
  GDT[3].type = SEG_TYPE_DATA;
  GDT[3].s = 1;
  GDT[3].p = 1;
  GDT[3].d = 1;
  GDT[3].g = 1;

  IDT[0x30].offset_lo = (u32)irq0;
  IDT[0x30].offset_hi = ((u32)irq0) >> 16;
  IDT[0x30].segment = 0x10;
  IDT[0x30].type = INTERRUPT_GATE;
  IDT[0x30].p = 1;

  IDT[0x31].offset_lo = (u32)irq1;
  IDT[0x31].offset_hi = ((u32)irq1) >> 16;
  IDT[0x31].segment = 0x10;
  IDT[0x31].type = INTERRUPT_GATE;
  IDT[0x31].p = 1;

  GDTR.base = (u32)GDT + SEGMENT_BASE_ADDRESS;
  GDTR.limit = (sizeof GDT) - 1;
  IDTR.base = (u32)IDT + SEGMENT_BASE_ADDRESS;
  IDTR.limit = (sizeof IDT) - 1;
}

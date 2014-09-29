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

#ifndef __DT_H__
#define __DT_H__

#include "types.h"

typedef struct {
  u16 seg_limit_lo;
  u16 base_lo;
  u8  base_mid;
  unsigned type:4;
#define SEG_TYPE_DATA 0x3
#define SEG_TYPE_CODE 0xb
  unsigned s:1;
  unsigned dpl:2;
  unsigned p:1;
  unsigned seg_limit_hi:4;
  unsigned avl:1;
  unsigned l:1;
  unsigned d:1;
  unsigned g:1;
  u8 base_hi;
} __attribute__ ((__packed__)) __attribute__((__aligned__(8))) gdt_entry;

typedef struct {
  u16 offset_lo;
  u16 segment;
  u8  unused;
  unsigned type:5;
#define INTERRUPT_GATE 0xe
#define TRAP_GATE      0xf
  unsigned dpl:2;
  unsigned p:1;
  u16 offset_hi;
} __attribute__ ((__packed__)) __attribute__((__aligned__(8))) idt_entry;

typedef struct {
  u16 limit;
  void *base;
} __attribute__ ((__packed__)) dtr;

#endif

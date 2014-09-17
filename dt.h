/*
 * Copyright (c) 2014 Nutanix Inc. All rights reserved.
 *
 * Author: cui@nutanix.com
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

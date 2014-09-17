/*
 * Copyright (c) 2014 Nutanix Inc. All rights reserved.
 *
 * Author: cui@nutanix.com
 *
 */

#ifndef  __VGA_H__
#define  __VGA_H__

#include "types.h"

extern void fb_init();
extern void set_pixel(int y, int x, u16 color);

static inline u16 rgb24to16(u32 rgb24)
{
  return (((rgb24 >> 8) & 0xf800) |
          ((rgb24 >> 5) & 0x07e0) |
          ((rgb24 >> 3) & 0x001f));
}

#endif

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

static inline u32 randint(u32 max)
{
  u32 q, r;
  const u32 PRIME = (1U << 31) - 1;

  // Compute TSC % PRIME.
  // X86 has a DIV instruction which can divide a u64 by u32 and return
  // both remainder and quotient. However GCC does not generate this
  // instruction and insists on calling into libgccc's _udiv64 function which
  // we don't have.
  asm volatile("rdtsc\n"
               "div %2" : "=&a"(q), "=&d"(r) : "r"(PRIME));
  return r % max;
}

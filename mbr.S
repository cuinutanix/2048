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

// Assembly for MBR or first 512 bytes of this program. Responsible for:
// 1. Loading the rest of the program into memory.
// 2. Setting up descriptors to enter 32-bit protected mode.
// 3. Jump into 32-bit C code.

.code16
.section .text.mbr_begin
.globl _entry
_entry:
        cli
        // Load 0 into segment registers to use linear address.
        xor %ax, %ax
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %ss
        mov $_entry, %sp

        ljmp $0, $1f // mov $0, %cs
1:
        // Read disk
        sti

.macro READ_DISK lba count
        mov $(\lba/63), %dh
        mov $(_entry + \lba*512), %ebx
        shr $4, %ebx
        mov %bx, %es
        xor %ebx, %ebx
        mov $(0x200 + \count), %ax
        mov $((\lba % 63)+1), %cx
        int $0x13
.endm
        READ_DISK   1, 62
        READ_DISK  63, 63
        READ_DISK 126, 63

        xor %ax, %ax
        mov %ax, %es

        // Attempt to switch to VBE mode 114h (800x600x16bit)
        mov $0x4f00, %ax
        mov $VBE_INFO, %edi
        int $0x10
        cmp $0x4f, %ax
        jne 2f

        mov $0x4f01, %ax
        mov $0x114,  %cx
        mov $VBE_MODE_INFO, %edi
        int $0x10
        cmp $0x4f, %ax
        jne 2f

        mov $0x4f02, %ax
        mov $0x4114, %bx
        int $0x10
        cmp $0x4f, %ax
        jne 2f

        movb $1, VBE_SUCCESS

2:
        xor %ax, %ax
        mov %ax, %es

        cli
        calll setup_descriptors // .code16gcc for setting up DTs

        lgdtl GDTR
        lidtl IDTR

        // Set bit-0 of %cr0 to enter protected mode.
        mov %cr0, %eax
        or $1, %eax
        mov %eax, %cr0

        // Load 32-bit code segment (0x10)
        ljmp $0x10, $_code32
_code32:
.code32
        // Load 32-bit data segments (0x18)
        mov $0x18, %eax
        mov %eax, %ds
        mov %eax, %es

        mov %eax, %ss
        mov $0xA0000, %esp // Stack grows down from 640K

        // ZERO out BSS
        xor %eax, %eax
        mov $_BSS_START, %edi
        mov $_BSS_END, %ecx
        sub %edi, %ecx
        rep stosb

        // Call into C code.
        call main

        // FREEZE!
        cli
        hlt

        // BIOS MBR signature, last 2 bytes of first sector.
.section .text.mbr_end
_mbr_sig:
        .byte 0x55
        .byte 0xaa

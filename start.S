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

_start:
#ifdef BOOT_OPROM
#include "oprom_header.S"
#endif

.code16
        cli
        cld
        // Stack grows down from A0000
        mov $0x9000, %ax
        mov %ax, %ss
        xor %esp, %esp
        mov %esp, %ebp

        // Relocate by adjusting real mode segment.
        mov %cs, %eax
        shl $4, %eax
        xor %ebx, %ebx
        call 1f
1:
        pop %bx
        add %eax, %ebx
        sub $1b, %ebx      // %ebx: linear base address
        mov %ebx, %eax
        shr $4, %eax
        mov %ax, %ds
        mov %ax, %es
        push %ax           // %cs
        push $1f           // %ip
        lret
1:

#ifdef BOOT_MBR
_read_disk:
        // Read disk
        sti

        test $0x80, %dl
        jz 1f // floppy
        // HDD
        pushw $63  // max sector
        pushw $254 // max head
        jmp 2f
1:
        pushw $18  // max sector
        pushw $1   // max head

2:
        xor %edi, %edi

        mov $2, %cx // cylinder = 0, sector = 2
        mov $0, %dh // head = 0

        leal (_start + 512)(%ebx), %eax
        shr $4, %eax
        mov %ax, %es

2:
        push %bx
        xor %bx, %bx    // es:bx = start address
        mov $0x201, %ax
        int $0x13
        pop %bx

        test %al, %al
1:      jz 1b

        add %al, %cl
        cmp -2(%bp), %cl
        jbe 1f

        mov $1, %cl
        inc %dh
        cmp -4(%bp), %dh
        jbe 1f

        mov $0, %dh
        inc %ch
1:
        push %cx
        movzbw %al, %ax
        mov %es, %cx
        shl $5, %ax
        add %cx, %ax
        mov %ax, %es
        leal _edata(%ebx), %ecx
        shr $4, %ecx
        cmp %ax, %cx
        pop %cx
        ja 2b

        mov %ds, %ax
        mov %ax, %es
#endif // if BOOT_MBR

_vbe_init:
        // Attempt to switch to VBE mode 114h (800x600x16bit)
        mov $0x4f00, %ax
        lea VBE_INFO, %di
        movl $('V' | ('B' << 8) | ('E' << 16) | ('2' << 24)), (%di)
        int $0x10
        cmp $0x4f, %ax
        jne 2f

        mov $0x4f01, %ax
        mov $0x114,  %cx
        lea VBE_MODE_INFO, %di
        int $0x10
        cmp $0x4f, %ax
        jne 2f

        mov $0x4f02, %ax
        push %bx
        mov $0x4114, %bx
        int $0x10
        pop %bx
        cmp $0x4f, %ax
        jne 2f

        movb $1, VBE_SUCCESS

2:
_protmode_init:
        cli
        mov %ebx, SEGMENT_BASE_ADDRESS
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
        sub %ebx, %esp     // Adjust for segment offset

        // Call into C code.
        call main

        // FREEZE!
1:
        cli
        hlt
        jmp 1b

#ifdef BOOT_MBR
2:
        .space (510 - (2b - _start)), 0x90
        .byte 0x55
        .byte 0xaa
#endif

.section .rodata
.global ERROR_MESSAGE
ERROR_MESSAGE:
#ifdef BOOT_MBR
        .string "Operating system not found."
#endif
#ifdef BOOT_OPROM
        .string "Boot device not found."
#endif

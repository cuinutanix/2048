/*
 * Copyright (c) 2015 Nutanix Inc. All rights reserved.
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

/*
A.2 PnP Option ROM Header
Offset Size Value Description
00h BYTE 55h Signature byte 1.
01h BYTE AAh Signature byte 2.
02h BYTE Varies Option ROM length in 512-byte blocks.
03h 4 BYTES Varies Initialization entry point.
07h 17 BYTES Varies Reserved.
18h WORD Varies Offset to PCI data structure.
1Ah WORD Varies Offset to expansion header structure.
*/
        .byte 0x55
        .byte 0xaa
_oprom_size:
        .byte 0x00 // Size, fill in later
        lret
        nop
        nop
        nop
        .space 17
        .short 0
        .short (_pnph - _start)

/*
A.3 PnP Expansion Header
Offset Size Value Description
00h BYTE '$' Signature byte 1.
01h BYTE 'P' Signature byte 2.
02h BYTE 'n' Signature byte 3.
03h BYTE 'P' Signature byte 4.
04h BYTE 01h Structure revision.
05h BYTE Varies Length (in 16 byte increments).
06h WORD Varies Offset of next header (0000h if none).
08h BYTE 00h Reserved.
09h BYTE Varies Checksum.
0Ah DWORD Varies Device identifier.
0Eh WORD Varies Pointer to manufacturer string (Optional).
10h WORD Varies Pointer to product name string (Optional).
12h 3 BYTES Varies Device type code.
15h BYTE Varies Device indicators.
16h WORD Varies Boot Connection Vector (BCV), 0000h if none.
18h WORD Varies Disconnect Vector (DV), 0000h if none.
1Ah WORD Varies Bootstrap Entry Vector (BEV), 0000h if none.
1Ch WORD 0000h Reserved.
1Eh WORD Varies Static resource information vector (0000h if none).
*/
_pnph:
        .byte '$'
        .byte 'P'
        .byte 'n'
        .byte 'P'
        .byte 1
        .byte (_pnph_end - _pnph) / 16
        .short 0
        .byte 0
_phph_checksum:
        .byte 0
        .long 0
_phph_manufacturer:
        .short (_manufacturer - _start)
_phph_product:
        .short (_product - _start)
        .byte 0
        .byte 0
        .byte 0
        .byte 0
        .short 0
        .short 0
_phph_bev:
        .short (_bev - _start)
        .short 0
        .short 0
_pnph_end:
_manufacturer:
        .string "Nutanix"
_product:
        .string "2048 Boot ROM"
_bev:

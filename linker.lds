ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
TARGET(elf32-i386)

SECTIONS
{
  . = 0x1000;
  _start = .;
  .text.realmode : { start.*.o(.text) realmode.o(.text) }
  .data.realmode : { realmode.o(SORT_BY_ALIGNMENT(.rodata*)
                                SORT_BY_ALIGNMENT(.data*)
                                SORT_BY_ALIGNMENT(.bss)) }
  .text : { *(.text) }
  .data : { *(SORT_BY_ALIGNMENT(.rodata*)
              SORT_BY_ALIGNMENT(.data)
              SORT_BY_ALIGNMENT(.bss)
              COMMON) }
  _edata = .;
  _end = .;
}

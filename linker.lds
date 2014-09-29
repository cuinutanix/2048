ENTRY(_entry)
OUTPUT_FORMAT(elf32-i386)
TARGET(elf32-i386)

SECTIONS
{
  . = 0x7c00;
  .text.realmode : { mbr.o(.text) realmode.o(.text) }
  .data.realmode : { realmode.o(.rodata*) realmode.o(.data) realmode.o(.bss) }
  .text : { *(.text) }
  .data : { *(.rodata*) *(.data) }
  _BSS_START = .;
  .bss : { *(.bss) *(COMMON) }
  _BSS_END = .;
}

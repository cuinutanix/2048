ENTRY(_entry)
OUTPUT_FORMAT(elf32-i386)
TARGET(elf32-i386)

SECTIONS
{
  . = 0x7c00;
  .text.mbr_begin : { *(.text.mbr_begin) }
  . = 0x7dfe;
  .text.mbr_end  : { *(.text.mbr_end) }
  .text : { *(.text) }
  .data : { *(.data.low) *(.rodata*) *(.data) }
  . = ALIGN(16);
  _BSS_START = .;
  .bss : { *(.bss) }
  _BSS_END = .;
}

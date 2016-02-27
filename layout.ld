ENTRY(_start)
SECTIONS
{
  . = 0x400000000;
  .text : { *(.text) }
  .rodata : { *(.rodata) }
}

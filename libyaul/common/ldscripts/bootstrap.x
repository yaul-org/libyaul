/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Anders Montonen
 */

OUTPUT_FORMAT ("binary")

SECTIONS
{
  PROVIDE_HIDDEN (__bootstrap_start = .);
  .rodata :
  {
     KEEP(*(.rodata))
  }
  PROVIDE_HIDDEN (__bootstrap_end = .);
  PROVIDE (__bootstrap_len = __bootstrap_end - __bootstrap_start);

 /DISCARD/ :
 {
   *(.text)
   *(.data)
   *(.bss)
 }
}

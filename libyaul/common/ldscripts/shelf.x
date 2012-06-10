/*
 * Copyright (c) 2011
 * See LICENSE for details.
 *
 * Joe Fenton <jlfenton65@gmail.com>
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

OUTPUT_FORMAT ("elf32-sh")
OUTPUT_ARCH (sh)
EXTERN (_start)
ENTRY (_start)

MEMORY {
  /* 0x06004000-0x7FFFFFFF */
  ram (wx) : ORIGIN = 0x06004000, LENGTH = 0x01FFC000
}

/* 0x06001000 */
PROVIDE_HIDDEN (_stack = ORIGIN (ram) - 0x3000);
/* 0x06000E00 */
PROVIDE_HIDDEN (_sstack = ORIGIN (ram) - 0x3200);

SECTIONS
{
  .text ORIGIN (ram) :
  AT (0x00000000)
  {
    PROVIDE_HIDDEN (__text_start = .);
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)

    . = ALIGN (16);
    __INIT_SECTION__ = .;
    KEEP (*(.init))
    SHORT (0x000B) /* RTS */
    SHORT (0x0009) /* NOP */
    . = ALIGN (16);
    __FINI_SECTION__ = .;
    KEEP (*(.fini))
    SHORT (0x000B) /* RTS */
    SHORT (0x0009) /* NOP */

    *(.eh_frame_hdr)
    KEEP (*(.eh_frame))
    *(.gcc_except_table)
    KEEP (*(.jcr))

    . = ALIGN (16);
     __CTOR_LIST__ = .;
    ___CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    LONG(0)
    __CTOR_END__ = .;

    . = ALIGN (16);
    __DTOR_LIST__ = .;
    ___DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
     LONG(0)
    __DTOR_END__ = .;

    *(.rdata)
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    . = ALIGN (16);
    PROVIDE_HIDDEN (__text_end = .);
  } > ram
  __text_size = __text_end - __text_start;

  .data :
  AT (LOADADDR (.text) + SIZEOF (.text))
  {
    __data_start = .;
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    CONSTRUCTORS

    *(.lit8)
    *(.lit4)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    . = ALIGN (16);
    __data_end = .;
  } > ram
  __data_size = __data_end - __data_start;

  .bss (NOLOAD) :
  {
    __bss_start = .;
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(COMMON)
    PROVIDE_HIDDEN (end = .);
    PROVIDE_HIDDEN (_end = ALIGN (16));
    PROVIDE_HIDDEN (__end = _end);
    __bss_end = .;
  } AT > ram
  __bss_size = __bss_end - __bss_start;

  /DISCARD/ :
  {
    *(.comment)
    *(.rela.*)
  }
}

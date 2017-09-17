/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Joe Fenton <jlfenton65@gmail.com>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

OUTPUT_FORMAT ("elf32-sh")
OUTPUT_ARCH (sh)
EXTERN (_start)
ENTRY (_start)
SEARCH_DIR ("$INSTALL_ROOT/sh-elf/lib");

MEMORY {
  /* 0x06004000-0x060FFFFF */
  ram (Wx) : ORIGIN = 0x06004000, LENGTH = 0x000F4000
}

/* 0x060F4000-0x06100000 */
PROVIDE (__stack = ORIGIN (ram) + LENGTH (ram) + 0x00008000);
PROVIDE (__stack_end = ORIGIN (ram) + LENGTH (ram));

SECTIONS
{
  .text ORIGIN (ram) :
  {
     PROVIDE_HIDDEN (__text_start = .);
     *(.text)
     *(.text.*)
     *(.gnu.linkonce.t.*)
     . = ALIGN (0x10);

     __INIT_SECTION__ = .;
     KEEP (*(.init))
     SHORT (0x000B) /* RTS */
     SHORT (0x0009) /* NOP */
     . = ALIGN (0x10);

     __FINI_SECTION__ = .;
     KEEP (*(.fini))
     SHORT (0x000B) /* RTS */
     SHORT (0x0009) /* NOP */
     . = ALIGN (0x10);

     __CTOR_LIST__ = .;
     ___CTOR_LIST__ = .;
     LONG (((__CTOR_END__ - __CTOR_LIST__) / 4) - 2)
     KEEP (*(SORT (.ctors.*)))
     KEEP (*(.ctors))
     LONG (0x00000000)
     __CTOR_END__ = .;
     . = ALIGN (0x04);

     __DTOR_LIST__ = .;
     ___DTOR_LIST__ = .;
     LONG (((__DTOR_END__ - __DTOR_LIST__) / 4) - 2)
     KEEP (*(SORT (.dtors.*)))
     KEEP (*(.dtors))
     LONG (0x00000000)
     __DTOR_END__ = .;
     . = ALIGN (0x04);

     *(.rdata)
     *(.rodata)
     *(.rodata.*)
     *(.gnu.linkonce.r.*)

     . = ALIGN (0x10);
     PROVIDE_HIDDEN (__text_end = .);
  }

  .data __text_end :
  {
     PROVIDE_HIDDEN (__data_start = .);
     *(.data)
     *(.data.*)
     *(.gnu.linkonce.d.*)
     SORT (CONSTRUCTORS)

     *(.sdata)
     *(.sdata.*)
     *(.gnu.linkonce.s.*)
     . = ALIGN (0x10);

     PROVIDE_HIDDEN (__data_end = .);
  }

  .bss __data_end :
  {
     PROVIDE (__bss_start = .);
     *(.bss)
     *(.bss.*)
     *(.gnu.linkonce.b.*)
     *(.sbss)
     *(.sbss.*)
     *(.gnu.linkonce.sb.*)
     *(.scommon)
     *(COMMON)
     . = ALIGN (0x10);

     PROVIDE (__bss_end = .);
     __bss_end__ = .;
  }

  __end = .;
  PROVIDE (_end = .);

 /DISCARD/ :
 {
    *(.rela.*)
    *(.comment)
 }
}

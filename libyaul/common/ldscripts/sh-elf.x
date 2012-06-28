/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Joe Fenton <jlfenton65@gmail.com>
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

OUTPUT_FORMAT ("elf32-sh")
OUTPUT_ARCH (sh)
EXTERN (_start)
ENTRY (_start)
SEARCH_DIR ("/path/to/sh-elf/sh-elf/lib");

MEMORY {
  /* 0x06004000-0x060FFFFF */
  ram (Wx) : ORIGIN = 0x06004000, LENGTH = 0x000FC000
}

/* 0x06100000-0x060FC000 */
PROVIDE (__stack = ORIGIN (ram) + LENGTH (ram));
PROVIDE (__stack_end = ORIGIN (ram) + LENGTH (ram) - 0x00004000);

SECTIONS
{
  .text ORIGIN (ram) : AT (0x00000000)
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
     . = ALIGN (0x04);

     __FINI_SECTION__ = .;
     KEEP (*(.fini))
     SHORT (0x000B) /* RTS */
     SHORT (0x0009) /* NOP */
     . = ALIGN (0x04);

     /* Constructor table */
     PROVIDE_HIDDEN (__preinit_array_start = .);
     KEEP (*(.preinit_array))
     PROVIDE_HIDDEN (__preinit_array_end = .);

     PROVIDE_HIDDEN (__init_array_start = .);
     KEEP (*(SORT(.init_array.*)))
     KEEP (*(.init_array))
     PROVIDE_HIDDEN (__init_array_end = .);

     PROVIDE_HIDDEN (__fini_array_start = .);
     KEEP (*(SORT(.fini_array.*)))
     KEEP (*(.fini_array))
     PROVIDE_HIDDEN (__fini_array_end = .);
     . = ALIGN (0x04);

     __CTOR_LIST__ = .;
     ___CTOR_LIST__ = .;
     LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
     KEEP (*(SORT(.ctors.*)))
     KEEP (*(.ctors))
     LONG(0x00000000)
     __CTOR_END__ = .;
     . = ALIGN (0x04);

     __DTOR_LIST__ = .;
     ___DTOR_LIST__ = .;
     LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
     KEEP (*(SORT(.dtors.*)))
     KEEP (*(.dtors))
     LONG(0x00000000)
     __DTOR_END__ = .;
     . = ALIGN (0x04);

     *(.rdata)
     *(.rodata)
     *(.rodata.*)
     *(.gnu.linkonce.r.*)

     . = ALIGN (0x10);
     PROVIDE_HIDDEN (__text_end = .);
  } > ram

  .data :
  AT (LOADADDR (.text) + SIZEOF (.text))
  {
     PROVIDE_HIDDEN (__data_start = .);
     *(.data)
     *(.data.*)
     *(.gnu.linkonce.d.*)
     SORT(CONSTRUCTORS)

     *(.sdata)
     *(.sdata.*)
     *(.gnu.linkonce.s.*)
     . = ALIGN (0x10);
  } > ram

  .bss (NOLOAD) :
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
  } AT > ram

  __end = .;
  PROVIDE (_end = .);

  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }
  .stab.excl 0 : { *(.stab.excl) }
  .stab.exclstr 0 : { *(.stab.exclstr) }
  .stab.index 0 : { *(.stab.index) }
  .stab.indexstr 0 : { *(.stab.indexstr) }
  .comment 0 : { *(.comment) }
  .line 0 : { *(.line) }
  .debug 0 : { *(.debug) }
  .debug_abbrev 0 : { *(.debug_abbrev) }
  .debug_aranges 0 : { *(.debug_aranges) }
  .debug_frame 0 : { *(.debug_frame) }
  .debug_funcnames 0 : { *(.debug_funcnames) }
  .debug_info 0 : { *(.debug_info) }
  .debug_line 0 : { *(.debug_line) }
  .debug_loc 0 : { *(.debug_loc) }
  .debug_macinfo 0 : { *(.debug_macinfo) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_sfnames 0 : { *(.debug_sfnames) }
  .debug_srcinfo 0 : { *(.debug_srcinfo) }
  .debug_str 0 : { *(.debug_str) }
  .debug_typenames 0 : { *(.debug_typenames) }
  .debug_varnames 0 : { *(.debug_varnames) }
  .debug_weaknames 0 : { *(.debug_weaknames) }

 /DISCARD/ :
 {
    *(.rela.*)
 }
}

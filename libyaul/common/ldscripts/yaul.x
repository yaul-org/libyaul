/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Joe Fenton <jlfenton65@gmail.com>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

OUTPUT_FORMAT ("elf32-sh")
OUTPUT_ARCH (sh)
EXTERN (_start)
ENTRY (_start)
SEARCH_DIR ("$YAUL_INSTALL_ROOT/$YAUL_ARCH_SH_PREFIX/$YAUL_ARCH_SH_PREFIX/lib");

MEMORY {
  boot (Wx)        : ORIGIN = 0x06000000, LENGTH = 0x00000C00
  master_stack (W) : ORIGIN = 0x06004000, LENGTH = 0x00002000
  slave_stack (W)  : ORIGIN = 0x06002000, LENGTH = 0x00001400
  ram (Wx)         : ORIGIN = 0x06004000, LENGTH = 0x000FC000
}

PROVIDE (___master_stack = ORIGIN (master_stack));
PROVIDE (___master_stack_end = ORIGIN (master_stack) - LENGTH (master_stack));
PROVIDE_HIDDEN (___slave_stack = ORIGIN (slave_stack));
PROVIDE_HIDDEN (___slave_stack_end = ORIGIN (slave_stack) - LENGTH (slave_stack));

SECTIONS
{
  .text ORIGIN (ram) :
  {
     PROVIDE_HIDDEN (__text_start = .);

     *(.text)
     *(.text.*)
     *(.gnu.linkonce.t.*)

     . = ALIGN (16);
     __CTOR_LIST__ = .;
     ___CTOR_LIST__ = .;
     LONG (((__CTOR_END__ - __CTOR_LIST__) / 4) - 2)
     KEEP (*(SORT (.ctors.*)))
     KEEP (*(.ctors))
     LONG (0x00000000)
     __CTOR_END__ = .;

     . = ALIGN (16);
     __DTOR_LIST__ = .;
     ___DTOR_LIST__ = .;
     LONG (((__DTOR_END__ - __DTOR_LIST__) / 4) - 2)
     KEEP (*(SORT (.dtors.*)))
     KEEP (*(.dtors))
     LONG (0x00000000)
     __DTOR_END__ = .;

     . = ALIGN (4);
     PROVIDE_HIDDEN (__text_end = .);
  }

  .rodata __text_end :
  {
     . = ALIGN (16);

     *(.rdata)
     *(.rodata)
     *(.rodata.*)
     *(.gnu.linkonce.r.*)

     . = ALIGN (4);
  }

  .data . :
  {
     . = ALIGN (16);
     PROVIDE_HIDDEN (__data_start = .);

     *(.data)
     *(.data.*)
     *(.gnu.linkonce.d.*)
     SORT (CONSTRUCTORS)
     *(.sdata)
     *(.sdata.*)
     *(.gnu.linkonce.s.*)

     . = ALIGN (4);
     PROVIDE_HIDDEN (__data_end = .);
  }

  .bss __data_end :
  {
     . = ALIGN (16);
     PROVIDE (__bss_start = .);

     *(.bss)
     *(.bss.*)
     *(.gnu.linkonce.b.*)
     *(.sbss)
     *(.sbss.*)
     *(.gnu.linkonce.sb.*)
     *(.scommon)
     *(COMMON)

     . = ALIGN (16);
     PROVIDE (__bss_end = .);

     __bss_end__ = .;
  }

  .uncached (0x20000000 | __bss_end) : AT (__bss_end)
  {
     PROVIDE_HIDDEN (__uncached_start = .);

     *(.uncached)
     *(.uncached.*)

     . = ALIGN (4);
     PROVIDE_HIDDEN (__uncached_end = .);
  }

  /* Back to cached addresses */
  __end = __bss_end + SIZEOF (.uncached);
}

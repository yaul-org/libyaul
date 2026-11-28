/*
 * Copyright (c)
 * See LICENSE for details.
 *
 * Joe Fenton <jlfenton65@gmail.com>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

OUTPUT_FORMAT ("elf32-sh")
OUTPUT_ARCH (sh)
EXTERN (_start)
ENTRY (_start)
SEARCH_DIR ("$YAUL_INSTALL_ROOT/$YAUL_ARCH_SH_PREFIX/lib");

MEMORY {
  ram (Wx) : ORIGIN = 0x06004000, LENGTH = 0x000FC000
}

/* Code and data in separate segments, so that none is writable and
 * executable. The flat binary is made from section addresses, which these do
 * not change */
PHDRS {
  text PT_LOAD FLAGS (5);               /* R, X */
  data PT_LOAD FLAGS (6);               /* R, W */
  uncached_data PT_LOAD FLAGS (6);      /* R, W */
  uncached_text PT_LOAD FLAGS (5);      /* R, X */
}

SECTIONS
{
  .text :
  {
     *(.text)
     *(.text.*)
     *(.gnu.linkonce.t.*)

     INCLUDE ldscripts/yaul-c++.x

     . = ALIGN (4);
  } :text

  .rodata :
  {
     . = ALIGN (16);

     *(.rdata)
     *(.rodata)
     *(.rodata.*)
     *(.gnu.linkonce.r.*)
  } :text

  .data :
  {
     . = ALIGN (16);

     *(.data)
     *(.data.*)
     *(.gnu.linkonce.d.*)
     SORT (CONSTRUCTORS)
     *(.sdata)
     *(.sdata.*)
     *(.gnu.linkonce.s.*)
  } :data

  .bss :
  {
     . = ALIGN (16);
     PROVIDE (___bss_start = .);

     *(.bss)
     *(.bss.*)
     *(.gnu.linkonce.b.*)
     *(.sbss)
     *(.sbss.*)
     *(.gnu.linkonce.sb.*)
     *(.scommon)
     *(COMMON)

     . = ALIGN (16);
     PROVIDE (___bss_end = .);
  } :data

  /* Uncached data, then uncached functions right after it, each in its own
   * segment */
  .uncached (0x20000000 | ___bss_end) : AT (___bss_end)
  {
     *(.uncached)
  } :uncached_data

  .uncached.function (ADDR (.uncached) + SIZEOF (.uncached)) :
    AT (LOADADDR (.uncached) + SIZEOF (.uncached))
  {
     *(.uncached.*)

     . = ALIGN (4);
  } :uncached_text

  /* Back to cached addresses */
  ___end = ___bss_end + SIZEOF (.uncached) + SIZEOF (.uncached.function);
}

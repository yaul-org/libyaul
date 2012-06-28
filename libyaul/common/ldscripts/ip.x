/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

OUTPUT_FORMAT ("binary")

MEMORY {
  ip (rx) : ORIGIN = 0x06002000, LENGTH = 0x00008000
}

SECTIONS
{
  .text ORIGIN (ip):
  {
     KEEP (*(.system_id))
     *(.text)
  } > ip

  PROVIDE_HIDDEN (__ip_len = SIZEOF (.text));

 /DISCARD/ :
 {
   *(.data)
   *(.bss)
 }
}

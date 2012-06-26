/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Anders Montonen
 */

.section ".rodata"
.align 1

.ascii "SEGA SEGASATURN "       /* Hardware ID */
.ascii "                "       /* Manufacturer ID */
.ascii "          "             /* Product number */
.ascii "V1.000"                 /* Version */
.ascii "yyyymmdd"               /* Release date */
.ascii "CD-1/1  "               /* Development info */
.ascii "JTUE      "             /* Area */
.ascii "      "
.ascii "J               "       /* Peripherals */
.ascii "                "       /* Title */
.ascii "                "
.ascii "                "
.ascii "                "
.ascii "                "
.ascii "                "
.ascii "                "
.ascii "                "
.long  __bootstrap_len          /* Length of bootstrap */
.long 0x06100000                /* Address of stack for master */
.long 0x00000000
.long 0x00000000                /* Address of stack for slave */
.long 0x06004000                /* Base address of executable */
.long 0x00000000                /* Length of executable */
.long 0x00000000
.long 0x00000000
.incbin "bootstrap/sys_sec.bin"
.incbin "bootstrap/sys_arej.bin"
.incbin "bootstrap/sys_aret.bin"
.incbin "bootstrap/sys_areu.bin"
.incbin "bootstrap/sys_aree.bin"
.incbin "bootstrap/sys_init.bin"

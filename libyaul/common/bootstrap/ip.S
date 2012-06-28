/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Anders Montonen
 */

.section ".system_id"
.align 1

.ascii "SEGA SEGASATURN "       /* Hardware identifier */
.ascii "SEGA TP KAISHA-A"       /* Manufacturer ID */
.ascii "T-99901G  "             /* Product number */
.ascii "V1.000"                 /* Version */
.ascii "19940912"               /* Release date */
.ascii "CD-1/1  "               /* Device information */
.ascii "JTUBKAEL  "             /* Target area symbol(s) */
.ascii "      "
.ascii "JAMKST          "       /* Compatible peripheral(s) */
.ascii "GAME TITLE      "       /* Game name */
.ascii "                "
.ascii "                "
.ascii "                "
.ascii "                "
.ascii "                "
.ascii "                "
.long 0x00000000                /* Reserved */
.long 0x00000000                /* Reserved */
.long 0x00000000                /* Reserved */
.long 0x00000000                /* Reserved */
.long __ip_len                  /* IP size */
.long 0x06100000                /* Stack-M */
.long 0x00000000                /* Reserved */
.long 0x00000000                /* Stack-S */
.LC0:
.long 0x06004000                /* 1st read address */
.long 0x00000000                /* 1st read size */
.long 0x00000000                /* Reserved */
.long 0x00000000                /* Reserved */
.incbin "bootstrap/sys_sec.bin"
.incbin "bootstrap/sys_arej.bin"
.incbin "bootstrap/sys_aret.bin"
.incbin "bootstrap/sys_areu.bin"
.incbin "bootstrap/sys_aree.bin"

.text
.align 1

        .incbin "bootstrap/sys_init.bin"

        mov.l .LC1, r1
        mov.l @r1, r1
        jmp @r1
        nop

.align 2

.LC1:
.long .LC0

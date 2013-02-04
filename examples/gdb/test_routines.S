/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _jsr
.type _jsr, @function

_jsr:
        mov #0x00, r0
        mov #0x01, r0
        mov #0x02, r0
        mov #0x03, r0
        rts
        nop

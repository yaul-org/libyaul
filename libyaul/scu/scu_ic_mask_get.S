/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _scu_ic_mask_get
.type _scu_ic_mask_get, @function

_scu_ic_mask_get:
        mov.l .LC1, r0
        mov.l @r0, r0
        rts
        nop

.align 2

.LC1:
.long 0x06000348

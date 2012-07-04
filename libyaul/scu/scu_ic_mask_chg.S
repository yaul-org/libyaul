/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _scu_ic_mask_chg
.type _scu_ic_mask_chg, @function

_scu_ic_mask_chg:
        mov.l .LC1, r1
        sts.l pr, @-r15
        jsr @r1
        nop
        lds.l @r15+, pr
        rts
        nop

.align 2

.LC1:
.long 0x060007C0

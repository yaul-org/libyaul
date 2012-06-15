/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _scu_ic_vct_get
.type _scu_ic_vct_get, @function

_scu_ic_vct_get:
        mov.l r14, @-r15
        sts.l pr, @-r15
        mov.l .LC1, r0
        mov.l @r0, r0
        rts
        nop
        lds.l @r15+, pr
        mov.l @r15+, r14
        rts
        nop

.align 2

.LC1:
        .long 0x06000304

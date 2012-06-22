/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _scu_ic_interrupt_get
.type _scu_ic_interrupt_get, @function

_scu_ic_interrupt_get:
        mov.l .LC1, r1
        sts.l pr, @-r15
        jsr @r1
        nop
        lds.l @r15+, pr
        rts
        nop

.align 2

.LC1:
.long 0x0600077C

/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _scu_ic_interrupt_set
.type _scu_ic_interrupt_set, @function

_scu_ic_interrupt_set:
        mov.l .LC1, r1
        mov.l r14, @-r15
        sts.l pr, @-r15
        jsr @r1
        nop
        lds.l @r15+, pr
        mov.l @r15+, r14
        rts
        nop

.align 2

.LC1:
        .long 0x06000794

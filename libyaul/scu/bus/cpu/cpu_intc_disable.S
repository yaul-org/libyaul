/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _cpu_intc_disable
.type _cpu_intc_disable, @function

_cpu_intc_disable:
        mov.l r14, @-r15
        sts.l pr, @-r15
        mov #0xF0, r1
        ldc r1, sr
        lds.l @r15+, pr
        mov.l @r15+, r14
        rts
        nop

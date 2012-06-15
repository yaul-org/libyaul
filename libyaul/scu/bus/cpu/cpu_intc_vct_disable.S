/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _cpu_intc_vct_disable
.type _cpu_intc_vct_disable, @function

_cpu_intc_vct_disable:
        mov.l r14, @-r15
        sts.l pr, @-r15
        mov r15, r14
        mov #0xF0, r0
        mov r14, r15
        lds.l @r15+, pr
        mov.l @r15+, r14
        ldc r0, sr
        rts
        nop

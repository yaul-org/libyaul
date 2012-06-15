/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _cpu_intc_vct_enable
.type _cpu_intc_vct_enable, @function

_cpu_intc_vct_enable:
        mov.l r14, @-r15
        sts.l pr, @-r15
        mov #0xFF, r2
        mov #0x0F, r1
        shll8 r2
        stc sr, r3
        or r2, r1
        and r1, r3
        ldc r3, sr
        lds.l @r15+, pr
        mov.l @r15+, r14
        rts
        nop

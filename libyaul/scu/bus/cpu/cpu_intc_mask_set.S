/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _cpu_intc_mask_set
.type _cpu_intc_mask_set, @function

_cpu_intc_mask_set:
        mov.l r14, @-r15
        sts.l pr, @-r15
        stc sr, r1
        mov #0xF0, r2
        shll2 r4
        shll2 r4
        and r2, r1
        or r4, r1
        ldc r1, sr
        lds.l @r15+, pr
        mov.l @r15+, r14
        rts
        nop

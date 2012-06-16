/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _cpu_intc_vector_base_get
.type _cpu_intc_vector_base_get, @function

_cpu_intc_vector_base_get:
        mov.l r14, @-r15
        sts.l pr, @-r15
        stc vbr, r0
        lds.l @r15+, pr
        mov.l @r15+, r14
        rts
        nop

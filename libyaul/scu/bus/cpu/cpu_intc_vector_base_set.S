/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _cpu_intc_vector_base_set
.type _cpu_intc_vector_base_set, @function

_cpu_intc_vector_base_set:
        ldc r4, vbr
        rts
        nop

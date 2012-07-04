/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_intc_vector_base_set
.type _cpu_intc_vector_base_set, @function

_cpu_intc_vector_base_set:
        ldc r4, vbr
        rts
        nop

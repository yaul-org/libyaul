/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_intc_vector_base_get
.type _cpu_intc_vector_base_get, @function

_cpu_intc_vector_base_get:
        stc vbr, r0
        rts
        nop

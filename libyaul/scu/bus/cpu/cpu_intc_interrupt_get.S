/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_intc_interrupt_get
.type _cpu_intc_interrupt_get, @function

_cpu_intc_interrupt_get:
        mov.l .LC1, r0
        shll2 r4
        mov.l @(r0, r4), r0
        rts
        nop

.align 2

.LC1:
.long _cpu_intc_vector_table

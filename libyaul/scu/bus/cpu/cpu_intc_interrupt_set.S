/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _cpu_intc_interrupt_set
.type _cpu_intc_interrupt_set, @function

_cpu_intc_interrupt_set:
        shll2 r4
        tst r5, r5
        bf .L2
        mov.l .LC1, r0
        mov.l @(r0, r4), r5
.L2:
        mov.l .LC2, r3
        mov.l .LC3, r1
        add r4, r1
        add r4, r3
        mov.l @r1, r2
        tst r2, r2
        bf .L3
        /* Initializing IHR for the first time */
        mov.l @r3, r2
        mov.l r2, @r1
        stc vbr, r0
        mov.l r2, @(r0, r4)
.L3:
        /* Update IHR */
        mov.l r5, @r3
        rts
        nop

.align 2

.LC1:
.long 0x20000600
.LC2:
.long _cpu_intc_vector_table
.LC3:
.long .LC4

.data
.align 2

.type .LC4, @object
.size .LC4, 1024

.LC4:
.space 1024, 0x00

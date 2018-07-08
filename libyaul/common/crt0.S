/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Joe Fenton <jlfenton65@gmail.com>
 * Israel Jacquez <mrkotfw@gmail.com>
 */

.text
.align 1

.global _start
.type _start, @function

_start:
        /* Set stack pointer for master CPU */
        mov.l .LC7, r15
        mov #0xF0, r1
        mov.l .LC0, r2

        /* Disable interrupts */
        mov.l .LC1, r3
        ldc r1, sr

        /* Clear 'BSS' section */
        mov r3, r8
        mov.l .LC3, r7
        mov #0x00, r5
        cmp/eq r2, r3
        bt .L2
.L1:
        mov.l r5, @-r3
        mov.l r5, @-r3
        mov.l r5, @-r3
        mov.l r5, @-r3
        cmp/eq r2, r3
        bf .L1
.L2:
        /* Initializer */
        jsr @r7
        nop

        /* Enable interrupts */
        mov #0xF0, r2
        mov.l .LC6, r1
        mov #0x11, r3
        ldc r2, sr
        mov.l .LC5, r0

        /* Purge and turn cache on as four-way set associative */
        mov.b r3, @r1

        /* Jump to user */
        mov #0x00, r4
        mov #0x00, r5
        mov #0x00, r6
        jsr @r0
        mov #0x00, r7

        /* Disable interrupts */
        mov #0xF0, r2
        mov.l .LC4, r1
        ldc r2, sr

        /* Finalizer */
        jmp @r1
        nop

.align 4

.LC0:
.long __bss_start

.LC1:
.long __bss_end

.LC3:
.long __INIT_SECTION__

.LC4:
.long __FINI_SECTION__

.LC5:
.long _main

.LC6:
.long 0xFFFFFE92

.LC7:
.long __stack

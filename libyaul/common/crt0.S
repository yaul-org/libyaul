/*
 * Copyright (c) 2012
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

        /* Disable interrupts */
        mov #0xF0, r1
        extu.b r1, r1
        stc sr, r2
        or r2, r1
        ldc r1, sr

        /* Clear 'BSS' section */
        mov #0x00, r1
        mov.l .LC0, r2
        mov.l .LC1, r3
        cmp/eq r2, r3
        bt .L2
.L1:
        mov.l r1, @r3
        mov.l r1, @-r3
        mov.l r1, @-r3
        mov.l r1, @-r3
        mov.l r1, @-r3
        cmp/eq r2, r3
        bf .L1
.L2:
        /* Initializer */
        mov.l .LC3, r1
        jsr @r1
        nop

        /* Purge and turn cache on as four-way set associative */
        mov.l .LC6, r1
        mov #0x11, r2
        mov.b r2, @r1

        /* Enable interrupts */
        mov #0xFF, r2
        mov #0x0F, r1
        shll8 r2
        stc sr, r3
        or r2, r1
        and r1, r3
        ldc r3, sr

        /* Jump to user */
        mov.l .LC4, r3
        jsr @r3
        nop

        /* Abort */
        mov.l .LC5, r1
        jmp @r1
        nop

.align 2

.LC0:
.long __bss_start

.LC1:
.long __bss_end

.LC3:
.long __INIT_SECTION__

.LC4:
.long _main

.LC5:
.long _abort

.LC6:
.long 0xFFFFFE92

.LC7:
.long __stack

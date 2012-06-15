/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Joe Fenton <jlfenton65@gmail.com>
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.text
.align 1

.global _start
.type _start, @function

_start:
        /* Set stack pointer for master CPU */
        mov.l .LC5, r15
        mov #0x00, r0
        lds r0, pr
        mov.l r14, @-r15
        sts.l pr, @-r15
        mov r15, r14

        /* Disable interrupts */
        mov #0xF0, r1
        ldc r1, sr

        /* Purge and turn cache off */
        mov.l .LC6, r3
        mov #0x10, r1
        mov.b r1, @r3

        /* Clear 'BSS' section */
        mov.l .LC1, r0
        mov.l .LC0, r1
        mov r0, r2
        sub r1, r0
        shlr2 r0
        shlr2 r0
        mov #0x00, r1
        cmp/gt r1, r0
        bf .L2
        nop
.L1:
        mov.l r1, @-r2
        mov.l r1, @-r2
        mov.l r1, @-r2
        mov.l r1, @-r2
        dt r0
        bf/s .L1
        nop
.L2:
        /* Initializer */
        mov.l .LC3, r1
        jsr @r1
        nop

        /* Purge and turn cache on */
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

        mov r14, r15
        lds.l @r15+, pr
        mov.l @r15+, r14
.L02:
        /* Abort */
        mov.l .LC4, r1
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
        .long __stack
.LC6:
        .long 0xFFFFFE92
.LC7:
        .long _abort

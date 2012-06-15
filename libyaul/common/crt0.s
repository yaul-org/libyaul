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
        mov.l .LC5, r15
        mov #0x00, r0
        lds r0, pr
        mov.l r14, @-r15
        sts.l pr, @-r15
        mov r15, r14
        /* disable interrupts */
        /* purge and turn cache off */
        mov.l .LC6, r3
        mov #0x10, r1
        mov.b r1, @r3
        /* clear 'bss' section */
        mov.l .LC1, r0
        mov.l .LC0, r1
        mov r0, r2
        sub r1, r0
        shlr2 r0
        shlr2 r0
        mov #0x00, r1
        cmp/gt r1, r0
        bf .l2
        nop
.l1:
        mov.l r1, @-r2
        mov.l r1, @-r2
        mov.l r1, @-r2
        dt r0
        bf/s .l1
        mov.l r1, @-r2
.l2:
        /* initializers */
        mov.l .LC3, r8
        mov.l .LC4, r9
        jsr @r8
        nop
        /* enable interrupts */
        /* purge and turn cache on */
        mov.l .LC6, r1
        mov #0x11, r0
        mov.b r0, @r1
        jsr @r9
        nop
        mov r14, r15
        lds.l @r15+, pr
        mov.l @r15+, r14
.l02:
        /* disable interrupts */
        bra .l02
        nop

.align 2

.LC0:
        .long __bss_start
.LC1:
        .long __bss_end
.LC3:
        .long __init_section__
.LC4:
        .long _main
.LC5:
        .long __stack
.LC6:
        .long 0xFFFFFE92

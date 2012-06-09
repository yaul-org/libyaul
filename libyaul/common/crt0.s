/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Joe Fenton <jlfenton65@gmail.com>
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _start
.TYPE _start, @function

_start:
        MOV.L .LC5,r15
        MOV #0x00,r0
        LDS r0,pr
        MOV.L r14,@-r15
        STS.L pr,@-r15
        MOV r15,r14
        /* Disable interrupts */
        /* Purge and turn cache off */
        MOV.L .LC6,r3
        MOV #0x10,r1
        MOV.B r1,@r3
        /* Clear 'BSS' section */
        MOV.L .LC1,r0
        MOV.L .LC0,r1
        MOV r0,r2
        SUB r1,r0
        SHLR2 r0
        SHLR2 r0
        MOV #0x00,r1
        CMP/GT r1,r0
        BF .L2
        NOP
.L1:
        MOV.L r1,@-r2
        MOV.L r1,@-r2
        MOV.L r1,@-r2
        DT r0
        BF/S .L1
        MOV.L r1,@-r2
.L2:
        /* Initializers */
        MOV.L .LC3,r8
        MOV.L .LC4,r9
        MOV.L .LC2,r10
        JSR @r8
        NOP
        /* Enable interrupts */
        /* Purge and turn cache on */
        MOV.L .LC6,r1
        MOV #0x11,r0
        MOV.B r0,@r1
        JSR @r9
        NOP
        /* Finishers */
        JSR @r10
        NOP
        MOV r14,r15
        LDS.L @r15+,pr
        MOV.L @r15+,r14
.L02:
        /* Disable interrupts */
        BRA .L02
        NOP

.ALIGN 2

.LC0:
        .LONG __bss_start
.LC1:
        .LONG _end
.LC2:
        .LONG __FINI_SECTION__
.LC3:
        .LONG __INIT_SECTION__
.LC4:
        .LONG _main
.LC5:
        .LONG _stack
.LC6:
        .LONG 0xFFFFFE92

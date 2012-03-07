/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _start

_start:
        MOV.L IMM_00,sp
        MOV.L IMM_03,r0
        MOV.L IMM_02,r1
        MOV r0,r2
        SUB r1,r0
        SHLR2 r0
        SHLR2 r0
        CMP/GT #0x00,r0
        BF BRA_01
        MOV #0x00,r1
BRA_00:
        MOV.L r1,@-r2
        MOV.L r1,@-r2
        MOV.L r1,@-r2
        DT r0
        BF/S BRA_00
        MOV.L r1,@-r2
BRA_01:
        MOV.L IMM_01,r1
        JMP @r1
        NOP

.ALIGN 4

IMM_00:
        .LONG _stack
IMM_01:
        .LONG _main
IMM_02:
        .LONG _bss
IMM_03:
        .LONG _end

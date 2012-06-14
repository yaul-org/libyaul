/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _scu_int_lvl_set
.TYPE _scu_int_lvl_set, @function

_scu_int_lvl_set:
        MOV.L r14,@-r15
        STS.L pr,@-r15
        MOV.L .LC1,r2
        MOV.L @r2,r2
        MOV #0x0F,r0
        AND r0,r4
        JMP @r2
        NOP
        LDS.L @r15+,pr
        MOV.L @r15+,r14
        RTS
        NOP

.ALIGN 2

.LC1:
        .LONG 0x06000280

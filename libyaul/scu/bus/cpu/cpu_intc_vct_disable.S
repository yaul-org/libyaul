/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _cpu_intc_vct_disable
.TYPE _cpu_intc_vct_disable, @function

_cpu_intc_vct_disable:
        MOV.L r14,@-r15
        STS.L pr,@-r15
        MOV r15,r14
        MOV #0xF0,r0
        MOV r14,r15
        LDS.L @r15+,pr
        MOV.L @r15+,r14
        LDC r0,sr
        RTS
        NOP

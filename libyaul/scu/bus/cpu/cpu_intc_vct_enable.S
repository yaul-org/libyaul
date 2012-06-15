/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _cpu_intc_vct_enable
.TYPE _cpu_intc_vct_enable, @function

_cpu_intc_vct_enable:
        MOV.L r14,@-r15
        STS.L pr,@-r15
        MOV #0xFF,r2
        MOV #0x0F,r1
        SHLL8 r2
        STC sr,r3
        OR r2,r1
        AND r1,r3
        LDC r3,sr
        LDS.L @r15+,pr
        MOV.L @r15+,r14
        RTS
        NOP

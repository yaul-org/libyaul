/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _cpu_intc_lvl_set
.TYPE _cpu_intc_lvl_set, @function

_cpu_intc_lvl_set:
        MOV.L r14,@-r15
        STS.L pr,@-r15
        SHLL2 r4
        SHLL2 r4
        STC sr,r0
        OR r4,r0
        LDC r0,sr
        LDS.L @r15+,pr
        MOV.L @r15+,r14
        RTS
        NOP

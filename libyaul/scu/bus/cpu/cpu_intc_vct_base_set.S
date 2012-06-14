/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _cpu_intc_vct_base_set
.TYPE _cpu_intc_vct_base_set, @function

_cpu_intc_vct_base_set:
        MOV.L r14,@-r15
        STS.L pr,@-r15
        LDC r4,vbr
        LDS.L @r15+,pr
        MOV.L @r15+,r14
        RTS
        NOP

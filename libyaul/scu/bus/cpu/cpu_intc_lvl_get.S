/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _cpu_intc_lvl_get
.TYPE _cpu_intc_lvl_get, @function

_cpu_intc_lvl_get:
        MOV.L r14,@-r15
        STS.L pr,@-r15
        STC sr,r0
        SHLR2 r0
        SHLR2 r0
        AND #0x0F,r0
        LDS.L @r15+,pr
        MOV.L @r15+,r14
        RTS
        NOP

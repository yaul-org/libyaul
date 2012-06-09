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
        STC sr,r0
        MOV #0xF0,r1
        NOT r1,r1
        AND r1,r0
        RTS
        LDC r0,sr

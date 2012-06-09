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
        MOV #0xF0,r0
        RTS
        LDC r0,sr

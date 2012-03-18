/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _cpu_intc_vct_disable

_cpu_intc_vct_disable:
        MOV #0xf0,r0
        RTS
        LDC r0,sr

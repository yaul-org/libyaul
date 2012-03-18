/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _cpu_intc_vct_enable

_cpu_intc_vct_enable:
        STC sr,r0
        MOV #0xf0,r1
        NOT r1,r1
        AND r1,r0
        RTS
        LDC r0,sr

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
        SHLL2 r4
        SHLL2 r4
        STC sr,r0
        OR r4,r0
        RTS
        LDC r0,sr

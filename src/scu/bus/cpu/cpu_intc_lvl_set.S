/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _cpu_ic_lvl_set

_cpu_ic_lvl_set:
        SHLL2 r4
        SHLL2 r4
        STC sr,r0
        OR r4,r0
        RTS
        LDC r0,sr

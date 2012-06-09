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
        STC sr,r0
        SHLR2 r0
        SHLR2 r0
        RTS
        AND #0x0F,r0

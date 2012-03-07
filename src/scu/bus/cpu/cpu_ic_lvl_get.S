/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _cpu_ic_lvl_get

_cpu_ic_lvl_get:
        STC sr,r0
        SHLR2 r0
        SHLR2 r0
        RTS
        AND #0x0f,r0

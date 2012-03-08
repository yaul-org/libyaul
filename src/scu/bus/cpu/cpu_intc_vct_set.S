/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _cpu_ic_vct_set

_cpu_ic_vct_set:
        RTS
        LDC r4,vbr

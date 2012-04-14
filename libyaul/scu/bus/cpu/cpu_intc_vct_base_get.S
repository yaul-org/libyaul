/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _cpu_intc_vct_base_get

_cpu_intc_vct_base_get:
        STC vbr,r0
        RTS
        NOP

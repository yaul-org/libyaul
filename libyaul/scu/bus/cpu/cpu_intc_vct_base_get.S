/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _cpu_intc_vct_base_get
.TYPE _cpu_intc_vct_base_get, @function

_cpu_intc_vct_base_get:
        STC vbr,r0
        RTS
        NOP

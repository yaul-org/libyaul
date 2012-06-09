/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _cpu_intc_vct_base_set
.TYPE _cpu_intc_vct_base_set, @function

_cpu_intc_vct_base_set:
        LDC r4,vbr
        RTS
        NOP

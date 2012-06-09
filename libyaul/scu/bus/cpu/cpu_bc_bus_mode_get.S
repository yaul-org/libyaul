/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _cpu_bc_bus_mode_get
.TYPE _cpu_bc_bus_mode_get, @function

_cpu_bc_bus_mode_get:
        MOV #0xE0,r0
        MOV.W .LC1,r1
        MOV.W @r0,r0
        AND r1,r0
        SHLL r0
        SHLR16 r0
        RTS
        NOP

.ALIGN 2

.LC1:
        .WORD 0x8000

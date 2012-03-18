/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _cpu_bc_bus_mode_get

_cpu_bc_bus_mode_get:
        MOV #0xe0,r0
        MOV.W IMM_00,r1
        MOV.W @r0,r0
        AND r1,r0
        SHLL r0
        RTS
        SHLR16 r0

.ALIGN 2

IMM_00:
        .WORD 0x8000

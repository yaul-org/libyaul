/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _scu_int_lvl_set

_scu_int_lvl_set:
        MOV #0x0f,r0
        AND r0,r4
        MOV.L IMM_00,r1 ! 0x26000280
        JMP @r1
        NOP

.ALIGN 4

IMM_00:
	.LONG 0x06000810

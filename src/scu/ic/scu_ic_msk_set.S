/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _scu_ic_msk_set

_scu_ic_msk_set:
	MOV.L IMM_00,r1 ! 0x26000340
	JMP @r1
	NOP

.ALIGN 4

IMM_00:
	.LONG 0x060007b0

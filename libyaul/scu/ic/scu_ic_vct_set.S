/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _scu_ic_vct_set

_scu_ic_vct_set:
        MOV.L IMM_00,r2
        MOV.L @r2,r2
        JMP @r2
        NOP

.ALIGN 2

IMM_00:
        .LONG 0x26000300

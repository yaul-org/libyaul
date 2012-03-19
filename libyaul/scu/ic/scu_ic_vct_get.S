/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _scu_ic_vct_get

_scu_ic_vct_get:
        MOV.L IMM_00,r0
        MOV.L @r0,r0
        RTS
        NOP

.ALIGN 2

IMM_00:
        .LONG 0x26000304

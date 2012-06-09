/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT
.ALIGN 1

.GLOBAL _scu_ic_vct_get
.TYPE _scu_ic_vct_get, @function

_scu_ic_vct_get:
        MOV.L .LC1,r0
        MOV.L @r0,r0
        RTS
        NOP

.ALIGN 2

.LC1:
        .LONG 0x26000304

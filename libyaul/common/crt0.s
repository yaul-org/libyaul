/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Chilly Willy
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.TEXT

.GLOBAL _start

_start:
        MOV.L stack,sp
        /* Disable interrupts */
        /* Purge and turn cache off */
        MOV.L CCTL,r3
        MOV #0x10,r1
        MOV.B r1,@r3
        /* Clear 'BSS' section */
        MOV.L end,r0
        MOV.L bss,r1
        MOV r0,r2
        SUB r1,r0
        SHLR2 r0
        SHLR2 r0
        MOV #0x00,r1
        CMP/GT r1,r0
        BF BRA_01
        NOP
BRA_00:
        MOV.L r1,@-r2
        MOV.L r1,@-r2
        MOV.L r1,@-r2
        DT r0
        BF/S BRA_00
        MOV.L r1,@-r2
BRA_01:
        /* Initializers */
        MOV.L fini,r0
        JSR @r0
        NOP
        /* Enable interrupts */
        /* Purge and turn cache on */
        MOV #0x11,r1
        MOV.B r1,@r3
        /* Jump */
        MOV.L main,r1
        JSR @r1
        NOP
        /* Finishers */
        MOV.L fini,r0
        JSR @r0
        NOP
BRA_02:
        /* Disable interrupts */
        BRA BRA_02
        NOP

.ALIGN 2

bss:
        .LONG __bss_start
end:
        .LONG _end
fini:
        .LONG __FINI_SECTION__
init:
        .LONG __INIT_SECTION__
main:
        .LONG _main
stack:
        .LONG _stack

CCTL:
        .LONG 0xfffffe92

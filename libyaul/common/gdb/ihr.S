/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "gdb-signals.h"

.macro MACRO_TRAMPOLINE_PROLOGUE
        /* Assuming SR and PC registers are already on the stack */
        sts.l pr, @-r15
        stc.l gbr, @-r15
        stc.l vbr, @-r15
        sts.l mach, @-r15
        sts.l macl, @-r15
        mov.l r14, @-r15
        mov.l r13, @-r15
        mov.l r12, @-r15
        mov.l r11, @-r15
        mov.l r10, @-r15
        mov.l r9, @-r15
        mov.l r8, @-r15
        mov.l r7, @-r15
        mov.l r6, @-r15
        mov.l r5, @-r15
        mov.l r4, @-r15
        mov.l r3, @-r15
        mov.l r2, @-r15
        mov.l r1, @-r15
        mov.l r0, @-r15
        mov r15, r1
        /* Restore the stack to what it was before */
        add #0x58, r1
        mov.l r1, @-r15
        /* Disable interrupts */
.endm

.macro MACRO_TRAMPOLINE_EPILOGUE
        mov.l @r15+, r1
        mov.l @r15+, r0
        mov.l @r15+, r1
        mov.l @r15+, r2
        mov.l @r15+, r3
        mov.l @r15+, r4
        mov.l @r15+, r5
        mov.l @r15+, r6
        mov.l @r15+, r7
        mov.l @r15+, r8
        mov.l @r15+, r9
        mov.l @r15+, r10
        mov.l @r15+, r11
        mov.l @r15+, r12
        mov.l @r15+, r13
        mov.l @r15+, r14
        lds.l @r15+, macl
        lds.l @r15+, mach
        ldc.l @r15+, vbr
        ldc.l @r15+, gbr
        lds.l @r15+, pr
        /* Enable interrupts */
        rte
        nop
.endm

.text
.align 1

.global _ihr_ubc
.type _ihr_ubc, @function

_ihr_ubc:
        rte
        nop

.global _ihr_break
.type _ihr_break, @function

_ihr_break:
        /* Sends a SIGTRAP to GDB */
        MACRO_TRAMPOLINE_PROLOGUE

        mov.l .LC1, r1
        /* ARG #1: PTR to register file */
        mov r15, r4
        /* ARG #2: GDB signal */
        mov #GDB_SIGTRAP, r5
        jsr @r1
        nop

        MACRO_TRAMPOLINE_EPILOGUE

.global _ihr_illegal_instruction
.type _ihr_illegal_instruction, @function

_ihr_illegal_instruction:
        /* Sends a SIGTRAP to GDB */
        MACRO_TRAMPOLINE_PROLOGUE

        mov.l .LC1, r1
        /* ARG #1: PTR to register file */
        mov r15, r4
        /* ARG #2: GDB signal */
        mov #GDB_SIGILL, r5
        jsr @r1
        nop

        MACRO_TRAMPOLINE_EPILOGUE

.global _ihr_illegal_slot
.type _ihr_illegal_slot, @function

_ihr_illegal_slot:
        bra _ihr_illegal_instruction
        nop

.global _ihr_cpu_address_error
.type _ihr_cpu_address_error, @function

_ihr_cpu_address_error:
        /* Sends a SIGTRAP to GDB */
        MACRO_TRAMPOLINE_PROLOGUE

        mov.l .LC1, r1
        /* ARG #1: PTR to register file */
        mov r15, r4
        /* ARG #2: GDB signal */
        mov #GDB_SIGSEGV, r5
        jsr @r1
        nop

        MACRO_TRAMPOLINE_EPILOGUE

.global _ihr_dma_address_error
.type _ihr_dma_address_error, @function

_ihr_dma_address_error:
        bra _ihr_cpu_address_error
        nop

.align 2

.LC1:
.long _gdb_monitor

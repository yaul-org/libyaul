/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.MACRO EXCEPTION_STUB handler
        STS.L pr, @-r15
        STC.L sr, @-r15
        STC.L gbr, @-r15
        STC.L vbr, @-r15
        STS.L mach, @-r15
        STS.L macl, @-r15
        MOV.L r14, @-r15
        MOV.L r13, @-r15
        MOV.L r12, @-r15
        MOV.L r11, @-r15
        MOV.L r10, @-r15
        MOV.L r9, @-r15
        MOV.L r8, @-r15
        MOV.L r7, @-r15
        MOV.L r6, @-r15
        MOV.L r5, @-r15
        MOV.L r4, @-r15
        MOV.L r3, @-r15
        MOV.L r2, @-r15
        MOV.L r1, @-r15
        MOV.L r0, @-r15
        MOV r15, r0
        ADD #0x5C, r0           ! Restore the stack to what it was before
        MOV.L r0, @-r15
        MOV r15, r4
        MOV.L \handler,r0
        JMP @r0
        NOP
.ENDM

.TEXT

.GLOBAL _exception_illegal_instruction

_exception_illegal_instruction:
        EXCEPTION_STUB exception_illegal_instruction

.GLOBAL _exception_illegal_slot

_exception_illegal_slot:
        EXCEPTION_STUB exception_illegal_slot

.GLOBAL _exception_cpu_address_error

_exception_cpu_address_error:
        EXCEPTION_STUB exception_cpu_address_error

.GLOBAL _exception_dma_address_error

_exception_dma_address_error:
        EXCEPTION_STUB exception_dma_address_error

.ALIGN 2

exception_illegal_instruction:
        .LONG _exception_handler_illegal_instruction

exception_illegal_slot:
        .LONG _exception_handler_illegal_slot

exception_cpu_address_error:
        .LONG _exception_handler_cpu_address_error

exception_dma_address_error:
        .LONG _exception_handler_dma_address_error

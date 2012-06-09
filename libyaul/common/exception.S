/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

.MACRO EXCEPTION_STUB handler
        STS.L pr,@-r15
        STC.L gbr,@-r15
        STC.L vbr,@-r15
        STS.L mach,@-r15
        STS.L macl,@-r15
        MOV.L r14,@-r15
        MOV.L r13,@-r15
        MOV.L r12,@-r15
        MOV.L r11,@-r15
        MOV.L r10,@-r15
        MOV.L r9,@-r15
        MOV.L r8,@-r15
        MOV.L r7,@-r15
        MOV.L r6,@-r15
        MOV.L r5,@-r15
        MOV.L r4,@-r15
        MOV.L r3,@-r15
        MOV.L r2,@-r15
        MOV.L r1,@-r15
        MOV.L r0,@-r15
        MOV r15,r0
        ADD #0x58,r0 ! Restore the stack to what it was before
        MOV.L r0,@-r15
        MOV r15,r4
        MOV.L \handler,r0
        JMP @r0
        NOP
.ENDM

.TEXT
.ALIGN 1

.GLOBAL _exception_illegal_instruction
.TYPE _exception_illegal_instruction, @function

_exception_illegal_instruction:
        EXCEPTION_STUB .LC1

.GLOBAL _exception_illegal_slot
.TYPE _exception_illegal_slot, @function

_exception_illegal_slot:
        EXCEPTION_STUB .LC2

.GLOBAL _exception_cpu_address_error
.TYPE _exception_cpu_address_error, @function

_exception_cpu_address_error:
        EXCEPTION_STUB .LC3

.GLOBAL _exception_dma_address_error
.TYPE _exception_dma_address_error, @function

_exception_dma_address_error:
        EXCEPTION_STUB .LC4

.ALIGN 2

.LC1:
        .LONG _exception_handler_illegal_instruction

.LC2:
        .LONG _exception_handler_illegal_slot

.LC3:
        .LONG _exception_handler_cpu_address_error

.LC4:
        .LONG _exception_handler_dma_address_error

/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <stdio.h>

#include <bios.h>
#include <cpu/intc.h>
#include <cpu/registers.h>
#include <cpu/map.h>

#include <internal.h>

extern void internal_exception_illegal_instruction(void);
extern void internal_exception_illegal_slot(void);
extern void internal_exception_cpu_address_error(void);
extern void internal_exception_dma_address_error(void);

static void _format_exception_message(struct cpu_registers *, char *, const char *);

static char _buffer[512];

void
cpu_init(void)
{
        /* Set hardware exception handling routines */
        cpu_intc_ihr_set(INTC_INTERRUPT_ILLEGAL_INSTRUCTION,
            internal_exception_illegal_instruction);
        cpu_intc_ihr_set(INTC_INTERRUPT_ILLEGAL_SLOT,
            internal_exception_illegal_slot);
        cpu_intc_ihr_set(INTC_INTERRUPT_CPU_ADDRESS_ERROR,
            internal_exception_cpu_address_error);
        cpu_intc_ihr_set(INTC_INTERRUPT_DMA_ADDRESS_ERROR,
            internal_exception_dma_address_error);

        /* Set the appropriate vector numbers for the on-chip peripheral
         * modules */
        MEMORY_WRITE(16, CPU(VCRA), (INTC_INTERRUPT_SCI_ERI << 8) | INTC_INTERRUPT_SCI_RXI);
        MEMORY_WRITE(16, CPU(VCRB), (INTC_INTERRUPT_SCI_TXI << 8) | INTC_INTERRUPT_SCI_TEI);
        MEMORY_WRITE(16, CPU(VCRC), (INTC_INTERRUPT_FRT_ICI << 8) | INTC_INTERRUPT_FRT_OCI);
        MEMORY_WRITE(16, CPU(VCRD), INTC_INTERRUPT_FRT_OVI << 8);
        MEMORY_WRITE(32, CPU(VCRDMA0), INTC_INTERRUPT_DMAC0);
        MEMORY_WRITE(32, CPU(VCRDMA1), INTC_INTERRUPT_DMAC1);
        MEMORY_WRITE(16, CPU(VCRWDT), (INTC_INTERRUPT_WDT_ITI << 8) | INTC_INTERRUPT_BSC);
        MEMORY_WRITE(32, CPU(VCRDIV), INTC_INTERRUPT_DIVU_OVFI);
}

void __noreturn
internal_ihr_exception_illegal_instruction(struct cpu_registers *regs)
{
        _format_exception_message(regs, _buffer, "Illegal instruction");

        internal_exception_show(_buffer);
}

void __noreturn
internal_ihr_exception_illegal_slot(struct cpu_registers *regs)
{
        _format_exception_message(regs, _buffer, "Illegal slot");

        internal_exception_show(_buffer);
}

void __noreturn
internal_ihr_exception_cpu_address_error(struct cpu_registers *regs)
{
        _format_exception_message(regs, _buffer, "CPU address error");

        internal_exception_show(_buffer);
}

void __noreturn
internal_ihr_exception_dma_address_error(struct cpu_registers *regs)
{
        _format_exception_message(regs, _buffer, "DMA address error");

        internal_exception_show(_buffer);
}

static void
_format_exception_message(struct cpu_registers *regs,
    char *buffer,
    const char *exception_name)
{
        (void)sprintf(buffer,
            "[1;44mException occurred: %s[m\n\n"
            "\t r0 = 0x%08X  r11 = 0x%08X\n"
            "\t r1 = 0x%08X  r12 = 0x%08X\n"
            "\t r2 = 0x%08X  r13 = 0x%08X\n"
            "\t r3 = 0x%08X  r14 = 0x%08X\n"
            "\t r4 = 0x%08X   sp = 0x%08X\n"
            "\t r5 = 0x%08X   sr = 0x%08X\n"
            "\t r6 = 0x%08X  gbr = 0x%08X\n"
            "\t r7 = 0x%08X  vbr = 0x%08X\n"
            "\t r8 = 0x%08X mach = 0x%08X\n"
            "\t r9 = 0x%08X macl = 0x%08X\n"
            "\tr10 = 0x%08X   pr = 0x%08X\n"
            "                     pc = 0x%08X\n",
            exception_name,
            (uintptr_t)regs->r[0], (uintptr_t)regs->r[11],
            (uintptr_t)regs->r[1], (uintptr_t)regs->r[12],
            (uintptr_t)regs->r[2], (uintptr_t)regs->r[13],
            (uintptr_t)regs->r[3], (uintptr_t)regs->r[14],
            (uintptr_t)regs->r[4], (uintptr_t)regs->sp,
            (uintptr_t)regs->r[5], (uintptr_t)regs->sr,
            (uintptr_t)regs->r[6], (uintptr_t)regs->gbr,
            (uintptr_t)regs->r[7], (uintptr_t)regs->vbr,
            (uintptr_t)regs->r[8], (uintptr_t)regs->mach,
            (uintptr_t)regs->r[9], (uintptr_t)regs->macl,
            (uintptr_t)regs->r[10], (uintptr_t)regs->pr,
            (uintptr_t)regs->pc);
}

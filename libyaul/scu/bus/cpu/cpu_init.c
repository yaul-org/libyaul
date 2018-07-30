/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <stdio.h>

#include <bios.h>

#include <cpu/divu.h>
#include <cpu/dmac.h>
#include <cpu/dual.h>
#include <cpu/frt.h>
#include <cpu/intc.h>
#include <cpu/map.h>
#include <cpu/registers.h>
#include <cpu/wdt.h>

#include <internal.h>

#define EXCEPTION_TRAMPOLINE_EMIT(name)                                        \
__asm__ (".align 4\n"                                                          \
         "\n"                                                                  \
         ".local __exception_" __STRING(name) "\n"                             \
         ".type __exception_" __STRING(name) ", @function\n"                   \
         "\n"                                                                  \
         "__exception_" __STRING(name) ":\n"                                   \
         "\tsts.l pr, @-r15\n"                                                 \
         "\tstc.l gbr, @-r15\n"                                                \
         "\tstc.l vbr, @-r15\n"                                                \
         "\tsts.l mach, @-r15\n"                                               \
         "\tsts.l macl, @-r15\n"                                               \
         "\tmov.l r14, @-r15\n"                                                \
         "\tmov #0xF0, r14\n"                                                  \
         "\tmov.l r13, @-r15\n"                                                \
         "\tldc r14, sr\n"                                                     \
         "\tmov.l 1f, r14\n"                                                   \
         "\tmov.l r12, @-r15\n"                                                \
         "\tmov r15, r13\n"                                                    \
         "\tmov.l r11, @-r15\n"                                                \
         "\tadd #0x28, r13\n"                                                  \
         "\tmov.l r10, @-r15\n"                                                \
         "\tmov.l r9, @-r15\n"                                                 \
         "\tmov.l r8, @-r15\n"                                                 \
         "\tmov.l r7, @-r15\n"                                                 \
         "\tmov.l r6, @-r15\n"                                                 \
         "\tmov.l r5, @-r15\n"                                                 \
         "\tmov.l r4, @-r15\n"                                                 \
         "\tmov.l r3, @-r15\n"                                                 \
         "\tmov.l r2, @-r15\n"                                                 \
         "\tmov.l r1, @-r15\n"                                                 \
         "\tmov.l r0, @-r15\n"                                                 \
         "\tmov.l r13, @-r15\n"                                                \
         "\tjmp @r14\n"                                                        \
         "\tmov r15, r4\n"                                                     \
         "\t.align 2\n"                                                        \
         "\n"                                                                  \
         "\t1:\n"                                                              \
         "\t.long __ihr_exception_" __STRING(name) "\n")

void _exception_illegal_instruction(void);
void _exception_illegal_slot(void);
void _exception_cpu_address_error(void);
void _exception_dma_address_error(void);

EXCEPTION_TRAMPOLINE_EMIT(illegal_instruction);
EXCEPTION_TRAMPOLINE_EMIT(illegal_slot);
EXCEPTION_TRAMPOLINE_EMIT(cpu_address_error);
EXCEPTION_TRAMPOLINE_EMIT(dma_address_error);

static void _format_exception_message(struct cpu_registers *, char *, const char *);

static char _buffer[1024];

void
cpu_init(void)
{
        /* Set hardware exception handling routines */
        cpu_intc_ihr_set(INTC_INTERRUPT_ILLEGAL_INSTRUCTION,
            _exception_illegal_instruction);
        cpu_intc_ihr_set(INTC_INTERRUPT_ILLEGAL_SLOT,
            _exception_illegal_slot);
        cpu_intc_ihr_set(INTC_INTERRUPT_CPU_ADDRESS_ERROR,
            _exception_cpu_address_error);
        cpu_intc_ihr_set(INTC_INTERRUPT_DMA_ADDRESS_ERROR,
            _exception_dma_address_error);

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

        cpu_divu_init();
        cpu_frt_init(FRT_CLOCK_DIV_8);
        cpu_wdt_init(WDT_CLOCK_DIV_2);
        cpu_dmac_init();
        cpu_dual_init();
}

static void __noreturn
_ihr_exception_illegal_instruction(struct cpu_registers *regs)
{
        _format_exception_message(regs, _buffer, "Illegal instruction");

        internal_exception_show(_buffer);
}

static void __noreturn
_ihr_exception_illegal_slot(struct cpu_registers *regs)
{
        _format_exception_message(regs, _buffer, "Illegal slot");

        internal_exception_show(_buffer);
}

static void __noreturn
_ihr_exception_cpu_address_error(struct cpu_registers *regs)
{
        _format_exception_message(regs, _buffer, "CPU address error");

        internal_exception_show(_buffer);
}

static void __noreturn
_ihr_exception_dma_address_error(struct cpu_registers *regs)
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
            "                     pc = 0x%08X\n"
            "----------------------------------------\n"
            "SR bits\n\n"
            "\tM Q I3 I2 I1 I0 - - S T\n"
            "\t%lu %lu  %lu  %lu  %lu  %lu     %lu %lu\n",
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
            (uintptr_t)regs->pc,
            (uint32_t)((regs->sr >> 9) & 0x01),
            (uint32_t)((regs->sr >> 8) & 0x01),
            (uint32_t)((regs->sr >> 7) & 0x01),
            (uint32_t)((regs->sr >> 6) & 0x01),
            (uint32_t)((regs->sr >> 5) & 0x01),
            (uint32_t)((regs->sr >> 4) & 0x01),
            (uint32_t)((regs->sr >> 1) & 0x01),
            (uint32_t)((regs->sr >> 0) & 0x01));
}

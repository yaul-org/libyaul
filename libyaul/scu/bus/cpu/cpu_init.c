/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 */

#include <sys/cdefs.h>

#include <stdio.h>
#include <stdlib.h>

#include <bios.h>

#include <cpu/divu.h>
#include <cpu/dmac.h>
#include <cpu/dual.h>
#include <cpu/frt.h>
#include <cpu/intc.h>
#include <cpu/map.h>
#include <cpu/registers.h>
#include <cpu/sci.h>
#include <cpu/wdt.h>

#include <vdp.h>
#include <dbgio/dbgio.h>

#include <internal.h>
#include <cpu-internal.h>
#include <vdp-internal.h>

static void _exception_message_puts(const cpu_registers_t * restrict regs,
    const char * restrict exception_name);

void
__cpu_init(void)
{
        extern void __exception_illegal_instruction(void);
        extern void __exception_illegal_slot(void);
        extern void __exception_cpu_address_error(void);
        extern void __exception_dma_address_error(void);

        /* Set hardware exception handling routines */
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_ILLEGAL_INSTRUCTION,
            __exception_illegal_instruction);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_CPU_ADDRESS_ERROR,
            __exception_illegal_slot);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_CPU_ADDRESS_ERROR,
            __exception_cpu_address_error);
        cpu_intc_ihr_set(CPU_INTC_INTERRUPT_DMA_ADDRESS_ERROR,
            __exception_dma_address_error);

        /* Set the appropriate vector numbers for the on-chip peripheral
         * modules */
        MEMORY_WRITE(16, CPU(VCRA), (CPU_INTC_INTERRUPT_SCI_ERI << 8) | CPU_INTC_INTERRUPT_SCI_RXI);
        MEMORY_WRITE(16, CPU(VCRB), (CPU_INTC_INTERRUPT_SCI_TXI << 8) | CPU_INTC_INTERRUPT_SCI_TEI);

        MEMORY_WRITE_AND(16, CPU(VCRWDT), ~0x007F);
        MEMORY_WRITE_OR(16, CPU(VCRWDT), CPU_INTC_INTERRUPT_BSC);

        __cpu_divu_init();
        cpu_frt_init(CPU_FRT_CLOCK_DIV_8);
        cpu_wdt_init(CPU_WDT_CLOCK_DIV_2);
        cpu_sci_init();
        __cpu_dmac_init();
        cpu_dual_comm_mode_set(CPU_DUAL_ENTRY_POLLING);
}

void __used
__exception_assert(const cpu_registers_t * restrict regs, const char * restrict exception_name)
{
        __reset();

        dbgio_init();
        dbgio_dev_deinit();
        dbgio_dev_default_init(DBGIO_DEV_VDP2);
        vdp2_tvmd_vblank_in_next_wait(1);
        dbgio_dev_font_load();

        dbgio_puts("[H[2J");

        _exception_message_puts(regs, exception_name);

        vdp2_tvmd_vblank_in_next_wait(1);
        dbgio_flush();
        __vdp2_commit(0);
        __vdp2_commit_wait(0);

        abort();
}

static void
_exception_message_puts(const cpu_registers_t * restrict regs,
    const char * restrict exception_name)
{
        dbgio_printf("[1;44mException occurred: %s[m\n", exception_name);

        dbgio_printf("\n"
            "   r0 = 0x%08X  r11 = 0x%08X\n"
            "   r1 = 0x%08X  r12 = 0x%08X\n"
            "   r2 = 0x%08X  r13 = 0x%08X\n"
            "   r3 = 0x%08X  r14 = 0x%08X\n"
            "   r4 = 0x%08X   sp = 0x%08X\n"
            "   r5 = 0x%08X   sr = 0x%08X\n"
            "   r6 = 0x%08X  gbr = 0x%08X\n"
            "   r7 = 0x%08X  vbr = 0x%08X\n"
            "   r8 = 0x%08X mach = 0x%08X\n"
            "   r9 = 0x%08X macl = 0x%08X\n"
            "  r10 = 0x%08X   pr = 0x%08X\n"
            "                     pc = 0x%08X\n",
            (uintptr_t)regs->r[0],  (uintptr_t)regs->r[11],
            (uintptr_t)regs->r[1],  (uintptr_t)regs->r[12],
            (uintptr_t)regs->r[2],  (uintptr_t)regs->r[13],
            (uintptr_t)regs->r[3],  (uintptr_t)regs->r[14],
            (uintptr_t)regs->r[4],  (uintptr_t)regs->sp,
            (uintptr_t)regs->r[5],  (uintptr_t)regs->sr,
            (uintptr_t)regs->r[6],  (uintptr_t)regs->gbr,
            (uintptr_t)regs->r[7],  (uintptr_t)regs->vbr,
            (uintptr_t)regs->r[8],  (uintptr_t)regs->mach,
            (uintptr_t)regs->r[9],  (uintptr_t)regs->macl,
            (uintptr_t)regs->r[10], (uintptr_t)regs->pr,
            (uintptr_t)regs->pc);

        dbgio_puts("----------------------------------------\n");

        dbgio_printf("SR bits\n"
            "\n"
            "\tM Q I3 I2 I1 I0 - - S T\n"
            "\t%lu %lu  %lu  %lu  %lu  %lu     %lu %lu\n",
            ((regs->sr >> 9) & 0x01UL),
            ((regs->sr >> 8) & 0x01UL),
            ((regs->sr >> 7) & 0x01UL),
            ((regs->sr >> 6) & 0x01UL),
            ((regs->sr >> 5) & 0x01UL),
            ((regs->sr >> 4) & 0x01UL),
            ((regs->sr >> 1) & 0x01UL),
            ((regs->sr >> 0) & 0x01UL));
}

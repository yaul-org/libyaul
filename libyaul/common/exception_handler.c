/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vdp2.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include <cons/vdp2.h>

#include "exception.h"

static void format(struct cpu_registers *, const char *);

void __attribute__ ((noreturn, visibility ("hidden")))
exception_ihr_illegal_instruction(struct cpu_registers *regs)
{

        format(regs, "Illegal instruction");
        abort();
}

void __attribute__ ((noreturn, visibility ("hidden")))
exception_ihr_illegal_slot(struct cpu_registers *regs)
{

        format(regs, "Illegal slot");
        abort();
}

void __attribute__ ((noreturn, visibility ("hidden")))
exception_ihr_cpu_address_error(struct cpu_registers *regs)
{

        format(regs, "CPU address error");
        abort();
}

void __attribute__ ((noreturn, visibility ("hidden")))
exception_ihr_dma_address_error(struct cpu_registers *regs)
{

        format(regs, "DMA address error");
        abort();
}

static void
format(struct cpu_registers *regs, const char *exception_name)
{
        static char buf[1024];

        uint16_t blcs_color[] = {
                0x80E0 /* Green */
        };

        struct cons cons;

        (void)sprintf(buf, "[1;44mException occurred:[m\n\t[1;44m%s[m\n\n"
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

        /* Reset the VDP2 */
        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x01FFFE),
            blcs_color, 0);

        cons_vdp2_init(&cons);
        cons_write(&cons, buf);
}

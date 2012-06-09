/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <vdp2/tvmd.h>
#include <vdp2/scrn.h>
#include <vdp2/vram.h>
#include <vdp2.h>

#include <cons/vdp2.h>

#include "exception.h"
#include "stack.h"

static void spin(void);
static void format(struct cpu_registers *, const char *);

void __attribute__ ((noreturn, visibility ("hidden")))
exception_handler_illegal_instruction(struct cpu_registers *regs)
{

        format(regs, "Illegal instruction");
        spin();
}

void __attribute__ ((noreturn, visibility ("hidden")))
exception_handler_illegal_slot(struct cpu_registers *regs)
{

        format(regs, "Illegal slot");
        spin();
}

void __attribute__ ((noreturn, visibility ("hidden")))
exception_handler_cpu_address_error(struct cpu_registers *regs)
{

        format(regs, "CPU address error");
        spin();
}

void __attribute__ ((noreturn, visibility ("hidden")))
exception_handler_dma_address_error(struct cpu_registers *regs)
{

        format(regs, "DMA address error");
        spin();
}

static void __attribute__ ((noreturn))
spin(void)
{
        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }
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
            "                     pc = 0x%08X\n"
            "[1B[1;44mStack backtrace[m\n%s",
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
            stack_backtrace());

        /* Reset the VDP2 */
        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x00000),
            blcs_color, 0);
        vdp2_tvmd_display_set(); /* Turn display ON */

        cons_vdp2_init(&cons);
        cons_write(&cons, buf);
}

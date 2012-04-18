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
#include <vdp2.h>

#include "exception.h"
#include "monitor.h"

#define BLCS_COL(x) (0x0001FFFE + (x))

static void spin(void);
static void format(struct cpu_registers *, const char *);

void __attribute__ ((noreturn))
exception_handler_illegal_instruction(struct cpu_registers *regs)
{

        format(regs, "Illegal instruction");
        spin();
}

void __attribute__ ((noreturn))
exception_handler_illegal_slot(struct cpu_registers *regs)
{

        format(regs, "Illegal slot");
        spin();
}

void __attribute__ ((noreturn))
exception_handler_cpu_address_error(struct cpu_registers *regs)
{

        format(regs, "CPU address error");
        spin();
}

void __attribute__ ((noreturn))
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
        char buf[1024];

        uint16_t blcs_color[] = {
                0x80E0 /* Green */
        };

        (void)sprintf(buf, "[01;44mException occurred:[00;00m\n\t[01;44m%s[00;00m\n\n"
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
            "                     pc = 0x%08X",
            exception_name,
            (unsigned int)regs->r[0], (unsigned int)regs->r[11],
            (unsigned int)regs->r[1], (unsigned int)regs->r[12],
            (unsigned int)regs->r[2], (unsigned int)regs->r[13],
            (unsigned int)regs->r[3], (unsigned int)regs->r[14],
            (unsigned int)regs->r[4], (unsigned int)regs->sp,
            (unsigned int)regs->r[5], (unsigned int)regs->sr,
            (unsigned int)regs->r[6], (unsigned int)regs->gbr,
            (unsigned int)regs->r[7], (unsigned int)regs->vbr,
            (unsigned int)regs->r[8], (unsigned int)regs->mach,
            (unsigned int)regs->r[9], (unsigned int)regs->macl,
            (unsigned int)regs->r[10], (unsigned int)regs->pr,
            (unsigned int)regs->pc);

        /* Reset the VDP2 */
        vdp2_init();
        vdp2_tvmd_display_set(); /* Turn display ON */
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, 3, BLCS_COL(0), blcs_color, 0);

        monitor_init();

        /* Wait until we can draw */
        vdp2_tvmd_vblank_in_wait();
        vdp2_tvmd_vblank_out_wait();
        vt100_write(monitor, buf);
}

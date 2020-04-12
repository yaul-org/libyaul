/*
 * Copyright (c) 2012 Theo Berkau
 * See LICENSE for details.
 *
 * Theo Beraku
 * Romulo Fernandes Machado <abra185@gmail.com>
 */

#include <cd-block.h>
#include <cpu/intc.h>

#include <stdbool.h>

#include "cd-block-internal.h"

static bool busy(void);

int
cd_block_cmd_execute(struct cd_block_regs *regs, struct cd_block_regs *status)
{
        uint16_t hirq;
        uint16_t cd_status;
        int error;

        uint32_t w;

        /* Disable interrupts */
        uint32_t mask;
        mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);
        error = -1;

        /* Check if we can continue */
        if (busy()) {
                goto busy;
        }

        hirq = MEMORY_READ(16, CD_BLOCK(HIRQ));
        hirq &= 0xFFFE;
        hirq &= ~regs->hirq_mask;
        MEMORY_WRITE(16, CD_BLOCK(HIRQ), hirq);

        MEMORY_WRITE(16, CD_BLOCK(CR1), regs->cr1);
        MEMORY_WRITE(16, CD_BLOCK(CR2), regs->cr2);
        MEMORY_WRITE(16, CD_BLOCK(CR3), regs->cr3);
        MEMORY_WRITE(16, CD_BLOCK(CR4), regs->cr4);

        /* Wait */
        for (w = 0; w < 0x00240000; w++) {
                /* Check if we can continue */
                if (!busy()) {
                        break;
                }
        }

        /* Check if we can continue */
        if (busy()) {
                goto busy;
        }

        /* Read status */
        status->cr1 = MEMORY_READ(16, CD_BLOCK(CR1));
        status->cr2 = MEMORY_READ(16, CD_BLOCK(CR2));
        status->cr3 = MEMORY_READ(16, CD_BLOCK(CR3));
        status->cr4 = MEMORY_READ(16, CD_BLOCK(CR4));

        /* Verify if the command executed was successful */
        cd_status = status->cr1;
        cd_status >>= 8;

        /* Checking if waiting or if command was rejected */
        if (cd_status == 0xFF || cd_status & 0x80) {
                goto busy;
        }

        error = 0;

busy:
        cpu_intc_mask_set(mask);

        /* Command executed was successful */
        return error;
}

static bool
busy(void)
{
        /* Is the CD-block busy? */
        return ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & CMOK) == 0x0000);
}


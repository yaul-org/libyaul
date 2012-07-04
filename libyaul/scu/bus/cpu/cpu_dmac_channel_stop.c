/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/dmac.h>
#include <cpu/intc.h>

#include <assert.h>

#include "cpu-internal.h"

void
cpu_dmac_channel_stop(void)
{
        uint32_t dmaor;
        uint32_t chcr0;
        uint32_t chcr1;

        /* When the AE bit is set to 1, DMA transfer cannot be enabled
         * even if the DE bit in the DMA channel control register is set
         *
         * When the NMIF bit is set to 1, DMA transfer cannot be enabled
         * even if the DE bit in the DMA channel control register is
         * set */
        dmaor = MEMORY_READ(32, CPU(DMAOR));
        dmaor &= ~0x0000000E;
        MEMORY_WRITE(32, CPU(DMAOR), dmaor);

        /* Immediately disable channels */
        chcr0 = MEMORY_READ(32, CPU(CHCR0));
        chcr0 &= ~0x00000001;
        MEMORY_WRITE(32, CPU(CHCR0), chcr0);

        chcr1 = MEMORY_READ(32, CPU(CHCR1));
        chcr1 &= ~0x00000001;
        MEMORY_WRITE(32, CPU(CHCR1), chcr1);
}

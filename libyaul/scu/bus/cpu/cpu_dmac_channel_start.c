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
cpu_dmac_channel_start(uint8_t ch)
{
        uint32_t chcr0;
        uint32_t chcr1;
        uint32_t dmaor;

        cpu_dmac_channel_stop();

        /* Read after stopping all DMA channels */
        dmaor = MEMORY_READ(32, CPU(DMAOR));

        switch (ch) {
        case CPU_DMAC_CHANNEL(0):
                chcr0 = MEMORY_READ(32, CPU(CHCR0));
                chcr0 |= 0x00000201;

                /* DMA transfers enabled on all channels */
                dmaor |= 0x00000001;

                /* Write to memory */
                MEMORY_WRITE(32, CPU(CHCR0), chcr0);
                break;
        case CPU_DMAC_CHANNEL(1):
                chcr1 = MEMORY_READ(32, CPU(CHCR1));
                chcr1 |= 0x00000201;

                /* Write to memory */
                MEMORY_WRITE(32, CPU(CHCR1), chcr1);
                break;
        default:
                assert((ch == CPU_DMAC_CHANNEL(0)) ||
                    (ch == CPU_DMAC_CHANNEL(1)));
                /* NOTREACHED */
        }

        /* Write to memory */
        MEMORY_WRITE(32, CPU(DMAOR), dmaor);
}

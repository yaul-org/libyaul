/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/dma.h>

#include <scu-internal.h>

void
scu_dma_cpu_level_stop(enum dma_level level)
{

        switch (level) {
        case DMA_LEVEL_0:
                MEMORY_WRITE(32, SCU(D0EN), 0x00000000);
                return;
        case DMA_LEVEL_1:
                MEMORY_WRITE(32, SCU(D1EN), 0x00000000);
                return;
        case DMA_LEVEL_2:
                MEMORY_WRITE(32, SCU(D2EN), 0x00000000);
                return;
        default:
                /* Panic */
                return;
        }
}

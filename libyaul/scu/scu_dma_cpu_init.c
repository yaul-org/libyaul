/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/dma.h>

#include <scu-internal.h>

void
scu_dma_cpu_init(void)
{

        /* Stop and disable all DMA levels */
        scu_dma_cpu_level_stop(DMA_LEVEL_0);
        scu_dma_cpu_level_stop(DMA_LEVEL_1);
        scu_dma_cpu_level_stop(DMA_LEVEL_2);
}

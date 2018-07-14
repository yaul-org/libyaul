/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/dma.h>

#include <scu-internal.h>

void
scu_dma_init(void)
{

        /* Stop and disable all DMA levels */
        scu_dma_level_stop(DMA_LEVEL_0);
        scu_dma_level_stop(DMA_LEVEL_1);
        scu_dma_level_stop(DMA_LEVEL_2);
}

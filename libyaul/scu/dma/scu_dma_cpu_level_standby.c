/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <dma/dma.h>

#include "dma_internal.h"

/*
 * Determine if a DMA level is on stand by
 */
bool
scu_dma_cpu_level_standby(enum dma_level lvl)
{
        register uint32_t r;
        register uint32_t op;

        r = MEM_READ(DSTA) >> 4;

        switch (lvl) {
        case DMA_LEVEL_0:
                /* Check if D0WT, D0MV, D0BK are set */
                op = 0x10030;
                break;
        case DMA_LEVEL_1:
                /* Check if D1WT, D1MV, D1BK are set */
                op = 0x20300;
                break;
        case DMA_LEVEL_2:
                /* Check if D2WT, D2MV, D2BK are set */
                op = 0x03000;
                break;
        default:
                return false;
        }

        return (r & op) == 0;
}

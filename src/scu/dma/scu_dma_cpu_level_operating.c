/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <scu/dma.h>

#include "dma_internal.h"

/*
 * Determine if a DMA level is operating
 */
bool
scu_dma_cpu_level_operating(enum dma_level lvl)
{
        register unsigned long *d;

        switch (lvl) {
        case DMA_LEVEL_0:
                return;
        case DMA_LEVEL_1:
                return;
        case DMA_LEVEL_2:
                return;
        default:
                /* Panic */
        }
}

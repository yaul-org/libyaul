/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <dma/dma.h>

#include "dma_internal.h"

/*
 * Determine if a DMA level is operating
 */
bool
scu_dma_cpu_level_operating(enum dma_level lvl)
{
        register uint32_t r;
        register uint32_t op;

        r = MEM_READ(DSTA) >> 4;
        op = 1 << (lvl << 2);
        return (r & op) == op;
}

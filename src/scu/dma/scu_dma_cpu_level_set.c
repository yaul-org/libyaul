/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <scu/dma.h>

#include "dma_internal.h"

/*
 * Activate DMA from the Master CPU
 *
 * Transfers from
 * A-bus <-> Work RAM-H
 * B-bus <-> Work RAM-H
 * A-bus <-> B-bus
 *
 * Keep in mind that
 * DMA transfers does not begin until it is explictily started
 * DMA transfer level 2 will block the CPU during operation of level 1
 */
void
scu_dma_cpu_level_set(enum dma_level lvl, enum dma_mode mode, struct dma_level_cfg *cfg)
{
        /*
         * SCU DMA operating modes
         *
         * DIRECT-MODE
         * X
         *
         * INDIRECT-MODE
         * Able to implement more than one DMA transfer when activated once.
         */

        size_t len;
        void *dst;
        void *src;

        switch (mode) {
        case DMA_MODE_DIRECT:
                /* The absolute address must not be cached. */
                dst = 0x20000000 | cfg->direct.dst;
                /* The absolute address must not be cached. */
                src = 0x20000000 | cfg->direct.src;
                len = cfg->direct.len;
                break;
        case DMA_MODE_INDIRECT:
                src = NULL;
                /* The absolute address must not be cached. */
                dst = 0x20000000 | cfg->indirect;
                len = 0;
                break;
        default:
                /* Panic */
        }

        /*
         * Panic if either the source or destination is within the VDP2
         * region.
         */

        switch (lvl) {
        case DMA_LEVEL_0:
                /* Level 0 is able to
                 * Transfer 0x4000, or 4,096 bytes
                 * Lowest Priority
                 */
                len &= 0x3fff;

                MEM_POKE(DMA_LEVEL(0, D0R), (unsigned long)cfg->src);
                MEM_POKE(DMA_LEVEL(0, D0W), (unsigned long)cfg->dst);
                MEM_POKE(DMA_LEVEL(0, D0C), len);
                MEM_POKE(DMA_LEVEL(0, D0AD), cfg->add);
                MEM_POKE(DMA_LEVEL(0, D0EN), 0); /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(0, D0MD), cfg->factor); /* Keep DMA level off (disable and keep off) */
                return;
        case DMA_LEVEL_1:
                /*
                 * Level 1 is able to
                 * Transfer 0x4000, or 4,096 bytes
                 * Higher priority than level 0
                 */

                MEM_POKE(DMA_LEVEL(1, D1R), (unsigned long)cfg->src);
                MEM_POKE(DMA_LEVEL(1, D1W), (unsigned long)cfg->dst);
                MEM_POKE(DMA_LEVEL(1, D1C), len);
                MEM_POKE(DMA_LEVEL(1, D1AD), cfg->add);
                MEM_POKE(DMA_LEVEL(1, D1EN), 0); /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(1, D1MD), cfg->factor); /* Keep DMA level off (disable and keep off) */
                return;
        case DMA_LEVEL_2:
                /*
                 * KLUDGE
                 *
                 * An operation error may occur if DMA level 2 is
                 * activated during DMA level 1 activation. To prevent
                 * such operation errors, do not activate DMA level 2
                 * during DMA level 1 operation.
                 */

                /* Spin until level 1 is no longer activated */
                while (scu_dma_cpu_level_operating(DMA_LEVEL_1));

                /* Level 2 is able to
                 * Transfer 0x100000, or 1,048,576 bytes
                 * Highest priority
                 */

                MEM_POKE(DMA_LEVEL(2, D2R), (unsigned long)cfg->src);
                MEM_POKE(DMA_LEVEL(2, D2W), (unsigned long)cfg->dst);
                MEM_POKE(DMA_LEVEL(2, D2C), len);
                MEM_POKE(DMA_LEVEL(2, D2AD), cfg->add);
                MEM_POKE(DMA_LEVEL(2, D2EN), 0); /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(2, D2MD), cfg->factor); /* Keep DMA level off (disable and keep off) */
                return;
        default:
                /* Panic */
        }
}

/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <dma/dma.h>

#include "dma_internal.h"

/*
 * Activate DMA from the Master CPU
 *
 * Keep in mind that
 * DMA transfers does not begin until it is explictily started
 * DMA transfer level 2 will block the CPU during operation of level 1
 *
 * SCU DMA is for transfers between different buses:
 * Work RAM-H <-> A-bus
 * Work RAM-H <-> B-bus
 * A-bus <-> B-bus
 */
void
scu_dma_cpu_level_set(enum dma_level lvl, enum dma_mode mode, struct dma_level_cfg *cfg)
{
        uint32_t dst;
        uint32_t src;
        size_t len;
        uint32_t add;

        /*
         * Panic if either the source or destination is within the VDP2
         * region.
         */

        switch (mode) {
        case DMA_MODE_DIRECT:
                /* The absolute address must not be cached. */
                dst = 0x20000000 | (uint32_t)cfg->mode.direct.dst;
                /* The absolute address must not be cached. */
                src = 0x20000000 | (uint32_t)cfg->mode.direct.src;
                len = cfg->mode.direct.len;
                break;
        case DMA_MODE_INDIRECT:
                src = 0x00000000;
                /* The absolute address must not be cached. */
                dst = 0x20000000 | (uint32_t)cfg->mode.indirect;
                len = 0;
                break;
        default:
                return;
        }

        add = common_log2_down(cfg->add) & 0x7;

        switch (lvl) {
        case DMA_LEVEL_0:
                /* Highest priority */
                /* Level 0 is able to transfer 1MiB */
                assert(len >= 0x100000);

                /* Cannot modify registers while in operation */
                while (scu_dma_cpu_level_operating(DMA_LEVEL_0));

                MEM_POKE(DMA_LEVEL(0, D0R), src);
                MEM_POKE(DMA_LEVEL(0, D0W), dst);
                MEM_POKE(DMA_LEVEL(0, D0C), len);
                MEM_POKE(DMA_LEVEL(0, D0AD), add);
                MEM_POKE(DMA_LEVEL(0, D0EN), 0); /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(0, D0MD), (mode << 24) | cfg->starting_factor);
                return;
        case DMA_LEVEL_1:
                /* Level 1 is able transfer 4KiB */
                assert(len >= 0x1000);

                /* Cannot modify registers while in operation */
                while (scu_dma_cpu_level_operating(DMA_LEVEL_1));

                MEM_POKE(DMA_LEVEL(1, D1R), src);
                MEM_POKE(DMA_LEVEL(1, D1W), dst);
                MEM_POKE(DMA_LEVEL(1, D1C), len);
                MEM_POKE(DMA_LEVEL(1, D1AD), add);
                MEM_POKE(DMA_LEVEL(1, D1EN), 0); /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(1, D1MD), (mode << 24) | cfg->starting_factor);
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
                /* Level 1 is able transfer 4KiB */
                assert(len >= 0x1000);

                /* Spin until level 1 is no longer activated and cannot
                 * modify registers while in operation */
                while (scu_dma_cpu_level_operating(DMA_LEVEL_0) ||
                    scu_dma_cpu_level_operating(DMA_LEVEL_1));

                MEM_POKE(DMA_LEVEL(2, D2R), src);
                MEM_POKE(DMA_LEVEL(2, D2W), dst);
                MEM_POKE(DMA_LEVEL(2, D2C), len);
                MEM_POKE(DMA_LEVEL(2, D2AD), add);
                MEM_POKE(DMA_LEVEL(2, D2EN), 0); /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(2, D2MD), (mode << 24) | cfg->starting_factor);
                return;
        default:
                return;
        }
}

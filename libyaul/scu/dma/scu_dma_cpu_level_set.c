/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <dma/dma.h>

#include "dma_internal.h"

/*
 * Items related to the entire SCU DMA
 * 01 02 04 08 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 35
 */

static int scu_dma_cpu_level_sanitize(struct dma_level_cfg *, enum dma_mode);

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

        if ((scu_dma_cpu_level_sanitize(cfg, mode)) < 0)
                return;

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
                dst = 0x20000000 | (uint32_t)cfg->mode.indirect.tbl;
                len = 0x00000000;
                break;
        default:
                return;
        }

        add = 0x00000100 | cfg->add;
        /* add = 0x0100 | (common_log2_down(cfg->add) & 0x7); */

        switch (lvl) {
        case DMA_LEVEL_0:
                /* Highest priority */
                /* Level 0 is able to transfer 1MiB */
                assert(len < 0x100000);

                /* Cannot modify registers while in operation */

                MEM_POKE(DMA_LEVEL(0, D0R), src);
                MEM_POKE(DMA_LEVEL(0, D0W), dst);
                /* Read of transfer byte count in DMA transfer register prohibited */
                MEM_POKE(DMA_LEVEL(0, D0C), len);
                MEM_POKE(DMA_LEVEL(0, D0AD), add);
                /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(0, D0EN), 0);
                MEM_POKE(DMA_LEVEL(0, D0MD), (mode << 24) | cfg->starting_factor);
                return;
        case DMA_LEVEL_1:
                /* Level 1 is able transfer 4KiB */
                assert(len < 0x1000);

                /* Cannot modify registers while in operation */

                MEM_POKE(DMA_LEVEL(1, D1R), src);
                MEM_POKE(DMA_LEVEL(1, D1W), dst);
                /* Read of transfer byte count in DMA transfer register prohibited */
                MEM_POKE(DMA_LEVEL(1, D1C), len);
                MEM_POKE(DMA_LEVEL(1, D1AD), add);
                /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(1, D1EN), 0x00000000);
                MEM_POKE(DMA_LEVEL(1, D1MD), (mode << 24) | cfg->starting_factor);
                return;
        case DMA_LEVEL_2:
                /*
                 * KLUDGE
                 *
                 * An operation error may occur if DMA level 2 is
                 * activated during DMA level 1 activation.
                 *
                 * To prevent such operation errors, do not activate DMA
                 * level 2 during DMA level 1 operation.
                 */
                /* Level 1 is able transfer 4KiB */
                assert(len < 0x1000);

                /* Spin until level 2 and level 1 are no longer
                 * activated.
                 *
                 * Level 2 cannot modify registers while in operation */

                MEM_POKE(DMA_LEVEL(2, D2R), src);
                MEM_POKE(DMA_LEVEL(2, D2W), dst);
                /* Read of transfer byte count in DMA transfer register prohibited */
                MEM_POKE(DMA_LEVEL(2, D2C), len);
                MEM_POKE(DMA_LEVEL(2, D2AD), add);
                /* Keep DMA level off (disable and keep off) */
                MEM_POKE(DMA_LEVEL(2, D2EN), 0x00000000);
                MEM_POKE(DMA_LEVEL(2, D2MD), (mode << 24) | cfg->starting_factor);
                return;
        default:
                return;
        }
}

static int
scu_dma_cpu_level_sanitize(struct dma_level_cfg *cfg, enum dma_mode mode)
{
        struct {
                uint32_t nelems; /* Upper bound of elements in a boundry */
                uint32_t boundry; /* Actual byte boundry */
        } tbl_nboundries[] = {
                {0x00000002, 0x0000001f},  {0x00000005, 0x0000003f},
                {0x0000000a, 0x0000007f},  {0x00000015, 0x000000ff},
                {0x0000002a, 0x000001ff},  {0x00000055, 0x000003ff},
                {0x000000aa, 0x000007ff},  {0x00000155, 0x00000fff},
                {0x000002aa, 0x00001fff},  {0x00000555, 0x00003fff},
                {0x00000aaa, 0x00007fff},  {0x00001555, 0x0000ffff},
                {0x00002aaa, 0x0001ffff},  {0x00005555, 0x0003ffff},
                {0x0000aaaa, 0x0007ffff},  {0x00015555, 0x000fffff},
                {0x0002aaaa, 0x001fffff},  {0x00055555, 0x003fffff},
                {0x000aaaaa, 0x007fffff},  {0x00155555, 0x00ffffff},
                {0x002aaaaa, 0x01ffffff},  {0x00555555, 0x03ffffff},
                {0x00aaaaaa, 0x07ffffff},  {0x01555555, 0x0fffffff},
                {0x02aaaaaa, 0x1fffffff},  {0x05555555, 0x3fffffff},
                {0x0aaaaaaa, 0x7fffffff},  {0x15555555, 0xffffffff},
                {0x00000000, 0x00000000}
        };

        uint32_t src;
        uint32_t dst;
        void *tbl;
        uint32_t nelems;

        int i;

        /*
         * 1. Check for prohitive areas
         *   1. A-bus write prohibited
         *   2. VDP2 area read prohibited
         *   4. WORKRAM-L usage from SCU disabled
         *
         * 2. Restriction on write address addition value in DMA based
         *    on access address
         *    WORKRAM-H                         -> 0x02 can be set
         *    External areas 1 to 3             -> 0x02 can be set
         *    External area 4 (A-bus I/O area)  -> 0x00 and 0x02 can be set
         *    VDP1, VDP2, SCSP                  -> all values can be set
         */

        switch (mode) {
        case DMA_MODE_DIRECT:
                src = (uint32_t)cfg->mode.direct.src & 0x0FFFFFFF;
                dst = (uint32_t)cfg->mode.direct.dst & 0x0FFFFFFF;

                if (((dst >= 0x00200000) && (dst <= 0x002FFFFF)) || /* WORKRAM-L */
                    ((src >= 0x05E00000) && (src <= 0x05FBFFFF)) || /* VDP2 area */
                    ((dst >= 0x02000000) && (dst <= 0x058FFFFF))) /* A-Bus CS0/1/dummy/2 regions */
                        return -1;
                return 0;
        case DMA_MODE_INDIRECT:
                nelems = cfg->mode.indirect.nelems;
                if (nelems == 0)
                        return -1;

                /*
                 * Major caveat
                 *
                 * The table start addresses must be placed on a
                 * 32-, 64-, 128-, 256-, 512-, 1024-, ...- byte boundary.
                 */
                tbl = cfg->mode.indirect.tbl;
                for (i = 0; tbl_nboundries[i].nelems; i++) {
                        if (nelems < tbl_nboundries[i].nelems) {
                                if (((uint32_t)tbl & tbl_nboundries[i].boundry))
                                        return -1;

                                return 0;
                        }
                }
                return 0;
        default:
                break;
        }

        return -1;
}

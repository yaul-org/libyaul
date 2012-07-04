/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/dma.h>

#include <scu-internal.h>

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

        add = 0x00000100 | (common_log2_down(cfg->add) & 0x7);

        switch (lvl) {
        case DMA_LEVEL_0:
                /* Highest priority */
                /* Level 0 is able to transfer 1MiB */
                assert(len < 0x100000);

                /* Cannot modify registers while in operation */

                MEMORY_WRITE(32, SCU(D0R), src);
                MEMORY_WRITE(32, SCU(D0W), dst);
                /* Read of transfer byte count in DMA transfer register prohibited */
                MEMORY_WRITE(32, SCU(D0C), len);
                MEMORY_WRITE(32, SCU(D0AD), add);
                /* Keep DMA level off (disable and keep off) */
                MEMORY_WRITE(32, SCU(D0EN), 0);
                MEMORY_WRITE(32, SCU(D0MD), (mode << 24) | cfg->starting_factor | cfg->update);
                return;
        case DMA_LEVEL_1:
                /* Level 1 is able transfer 4KiB */
                assert(len < 0x1000);

                /* Cannot modify registers while in operation */

                MEMORY_WRITE(32, SCU(D1R), src);
                MEMORY_WRITE(32, SCU(D1W), dst);
                /* Read of transfer byte count in DMA transfer register prohibited */
                MEMORY_WRITE(32, SCU(D1C), len);
                MEMORY_WRITE(32, SCU(D1AD), add);
                /* Keep DMA level off (disable and keep off) */
                MEMORY_WRITE(32, SCU(D1EN), 0x00000000);
                MEMORY_WRITE(32, SCU(D1MD), (mode << 24) | cfg->starting_factor | cfg->update);
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

                MEMORY_WRITE(32, SCU(D2R), src);
                MEMORY_WRITE(32, SCU(D2W), dst);
                /* Read of transfer byte count in DMA transfer register prohibited */
                MEMORY_WRITE(32, SCU(D2C), len);
                MEMORY_WRITE(32, SCU(D2AD), add);
                /* Keep DMA level off (disable and keep off) */
                MEMORY_WRITE(32, SCU(D2EN), 0x00000000);
                MEMORY_WRITE(32, SCU(D2MD), (mode << 24) | cfg->starting_factor | cfg->update);
        default:
                return;
        }
}

static int
scu_dma_cpu_level_sanitize(struct dma_level_cfg *cfg, enum dma_mode mode)
{
        /*
         * Items related to the entire SCU DMA
         * 01 02 04 08 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 35
         */
        struct {
                uint32_t nelems; /* Upper bound of elements in a boundry */
                uint32_t boundry; /* Actual byte boundry */
        } tbl_nboundries[] = {
                {0x00000002, 0x0000001F},  {0x00000005, 0x0000003F},
                {0x0000000A, 0x0000007F},  {0x00000015, 0x000000FF},
                {0x0000002A, 0x000001FF},  {0x00000055, 0x000003FF},
                {0x000000AA, 0x000007FF},  {0x00000155, 0x00000FFF},
                {0x000002AA, 0x00001FFF},  {0x00000555, 0x00003FFF},
                {0x00000AAA, 0x00007FFF},  {0x00001555, 0x0000FFFF},
                {0x00002AAA, 0x0001FFFF},  {0x00005555, 0x0003FFFF},
                {0x0000AAAA, 0x0007FFFF},  {0x00015555, 0x000FFFFF},
                {0x0002AAAA, 0x001FFFFF},  {0x00055555, 0x003FFFFF},
                {0x000AAAAA, 0x007FFFFF},  {0x00155555, 0x00FFFFFF},
                {0x002AAAAA, 0x01FFFFFF},  {0x00555555, 0x03FFFFFF},
                {0x00AAAAAA, 0x07FFFFFF},  {0x01555555, 0x0FFFFFFF},
                {0x02AAAAAA, 0x1FFFFFFF},  {0x05555555, 0x3FFFFFFF},
                {0x0AAAAAAA, 0x7FFFFFFF},  {0x15555555, 0xFFFFFFFF},
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

                if (((src >= 0x00200000) && (src <= 0x002FFFFF)) || /* WORKRAM-L */
                    ((dst >= 0x00200000) && (dst <= 0x002FFFFF)) ||
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
                /* NOTREACHED */
                break;
        }

        return 0;
}

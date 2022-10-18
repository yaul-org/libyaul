/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/cache.h>
#include <cpu/instructions.h>

#include <vdp1/cmdt.h>
#include <vdp1/vram.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "vdp-internal.h"

vdp1_cmdt_list_t *
vdp1_cmdt_list_alloc(uint16_t count)
{
        assert(count > 0);

        vdp1_cmdt_list_t *cmdt_list;
        cmdt_list = malloc(sizeof(vdp1_cmdt_list_t));
        assert(cmdt_list != NULL);

        vdp1_cmdt_t *cmdts;
        cmdts = memalign(count * sizeof(vdp1_cmdt_t), sizeof(vdp1_cmdt_t));
        assert(cmdts != NULL);

        vdp1_cmdt_list_init(cmdt_list, cmdts);

        return cmdt_list;
}

void
vdp1_cmdt_list_free(vdp1_cmdt_list_t *cmdt_list)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);

        cmdt_list->count = 0;

        free(cmdt_list->cmdts);
        free(cmdt_list);
}

void
vdp1_cmdt_list_init(vdp1_cmdt_list_t *cmdt_list, vdp1_cmdt_t *cmdts)
{
        assert(cmdt_list != NULL);
        assert(cmdts != NULL);

        cmdt_list->cmdts = cmdts;
        cmdt_list->count = 0;
}

vdp1_cmdt_orderlist_t *
vdp1_cmdt_orderlist_alloc(uint16_t count)
{
        assert(count > 0);

        const uint32_t size = count * sizeof(vdp1_cmdt_t);

        uint32_t aligned_boundary;
        aligned_boundary = uint32_log2(size);

        if ((aligned_boundary & (aligned_boundary - 1)) != 0x00000000) {
                aligned_boundary++;
        }

        vdp1_cmdt_orderlist_t * const cmdt_orderlist =
            memalign(sizeof(vdp1_cmdt_orderlist_t), 1 << aligned_boundary);
        assert(cmdt_orderlist != NULL);

        vdp1_cmdt_orderlist_init(cmdt_orderlist, count);

        return cmdt_orderlist;
}

void
vdp1_cmdt_orderlist_free(vdp1_cmdt_orderlist_t *cmdt_orderlist)
{
        assert(cmdt_orderlist != NULL);

        free(cmdt_orderlist);
}

void
vdp1_cmdt_orderlist_init(vdp1_cmdt_orderlist_t *cmdt_orderlist, uint16_t count)
{
        assert(cmdt_orderlist != NULL);
        assert(count > 0);

        scu_dma_xfer_t *xfer_table;
        xfer_table = (scu_dma_xfer_t *)cmdt_orderlist;

        for (uint32_t i = 0; i < count; i++) {
                xfer_table->len = sizeof(vdp1_cmdt_t);
                xfer_table->dst = 0x00000000;
                xfer_table->src = 0x00000000;

                xfer_table++;
        }
}

void
vdp1_cmdt_orderlist_vram_patch(vdp1_cmdt_orderlist_t *cmdt_orderlist,
    const vdp1_cmdt_t *cmdt_base, uint16_t count)
{
        assert(cmdt_orderlist != NULL);
        assert(count > 0);

        scu_dma_xfer_t * const xfer_table =
            (scu_dma_xfer_t *)cmdt_orderlist;

        for (uint32_t i = 0; i < count; i++) {
                xfer_table[i].dst = (uintptr_t)cmdt_base + (i * sizeof(vdp1_cmdt_t));
        }

        xfer_table[count - 1].len |= SCU_DMA_INDIRECT_TABLE_END;
}

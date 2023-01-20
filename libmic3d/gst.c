/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <math.h>

#include "internal.h"

#define ADDRESS_SPACE_REGION(x) ((((uintptr_t)(x)) >> 20) & 0x0F)

void
__gst_init(void)
{
        gst_unset();
}

void
gst_set(vdp1_vram_t vram_base)
{
        gst_t * const gst = __state.gst;

        gst->vram_base = vram_base;
        gst->slot_base = vram_base >> 3;
}

void
gst_unset(void)
{
        gst_set(0x00000000);
}

void
gst_put(const vdp1_gouraud_table_t *gouraud_tables, uint32_t put_count)
{
        gst_t * const gst = __state.gst;

        __gst_put(gouraud_tables, gst->vram_base, put_count);
}

void
gst_put_wait(void)
{
        gst_t * const gst = __state.gst;

        if (gst->dma_channel < CPU_DMAC_CHANNEL_COUNT) {
                cpu_dmac_transfer_wait(gst->dma_channel);
        } else if (gst->dma_level < SCU_DMA_LEVEL_COUNT) {
                scu_dma_transfer_wait(gst->dma_level);
        }
}

void
__gst_put(const vdp1_gouraud_table_t *gouraud_tables, vdp1_vram_t vram_base, uint32_t put_count)
{
        if (put_count == 0) {
                return;
        }

        gst_t * const gst = __state.gst;

        const uint32_t address_space = ADDRESS_SPACE_REGION(gouraud_tables);

        if (address_space == ADDRESS_SPACE_REGION(HWRAM(0))) {
                gst->dma_level = max(0, (int32_t)scu_dma_level_unused_get());
                gst->dma_channel = -1;

                scu_dma_transfer(gst->dma_level, (void *)vram_base, gouraud_tables, put_count * sizeof(vdp1_gouraud_table_t));
        } else if (address_space == ADDRESS_SPACE_REGION(LWRAM(0))) {
                gst->dma_channel = 0;
                gst->dma_level = -1;

                cpu_dmac_transfer(gst->dma_channel, (void *)vram_base, gouraud_tables, put_count * sizeof(vdp1_gouraud_table_t));
        }
}

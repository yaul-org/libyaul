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
        __state.gst->vram_base = vram_base;
        __state.gst->slot_base = vram_base >> 3;
}

void
gst_unset(void)
{
        gst_set(0x00000000);
}

void
gst_put(const vdp1_gouraud_table_t *gouraud_tables, uint32_t put_count)
{
        __gst_put(gouraud_tables, __state.gst->vram_base, put_count);
}

void
gst_put_wait(void)
{
        if (__state.gst->dma_channel >= 0) {
                cpu_dmac_transfer_wait(__state.gst->dma_channel);
        } else if (__state.gst->dma_level >= 0) {
                scu_dma_transfer_wait(__state.gst->dma_level);
        }
}

void
__gst_put(const vdp1_gouraud_table_t *gouraud_tables, vdp1_vram_t vram_base, uint32_t put_count)
{
        if (put_count == 0) {
                return;
        }

        const uint32_t address_space = ADDRESS_SPACE_REGION(gouraud_tables);

        if (address_space == ADDRESS_SPACE_REGION(HWRAM(0))) {
                __state.gst->dma_level = max(0, scu_dma_level_unused_get());
                __state.gst->dma_channel = -1;

                scu_dma_transfer(__state.gst->dma_level, (void *)vram_base, gouraud_tables, put_count * sizeof(vdp1_gouraud_table_t));
        } else if (address_space == ADDRESS_SPACE_REGION(LWRAM(0))) {
                __state.gst->dma_channel = 0;
                __state.gst->dma_level = -1;

                cpu_dmac_transfer(__state.gst->dma_channel, (void *)vram_base, gouraud_tables, put_count * sizeof(vdp1_gouraud_table_t));
        }
}

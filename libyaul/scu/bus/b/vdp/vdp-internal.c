/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "vdp-internal.h"

#include <cpu/cache.h>

struct state_vdp1 __state_vdp1;
struct state_vdp2 __state_vdp2;

static scu_dma_handle_t _commit_dma_handle;
static scu_dma_xfer_t _commit_dma_xfer_table[COMMIT_XFER_COUNT] __aligned(COMMIT_XFER_TABLE_ALIGNMENT);

void
__vdp2_xfer_table_init(void)
{
        _state_vdp2()->commit.dma_handle = &_commit_dma_handle;
        _state_vdp2()->commit.xfer_table = &_commit_dma_xfer_table[0];

        __vdp2_xfer_table_update(COMMIT_XFER_VDP2_REG_TVMD);
        __vdp2_xfer_table_update(COMMIT_XFER_VDP2_REGS);
        __vdp2_xfer_table_update(COMMIT_XFER_BACK_SCREEN);

        scu_dma_xfer_t * const xfer_table =
            &_state_vdp2()->commit.xfer_table[0];

        const scu_dma_level_cfg_t dma_cfg = {
                .mode          = SCU_DMA_MODE_INDIRECT,
                .xfer.indirect = xfer_table,
                .space         = SCU_DMA_SPACE_BUS_B,
                .stride        = SCU_DMA_STRIDE_2_BYTES,
                .update        = SCU_DMA_UPDATE_NONE
        };

        scu_dma_handle_t * const dma_handle =
            _state_vdp2()->commit.dma_handle;

        scu_dma_config_buffer(dma_handle, &dma_cfg);
}

void
__vdp2_xfer_table_update(uint32_t xfer_index)
{
        scu_dma_xfer_t * const xfer =
            &_state_vdp2()->commit.xfer_table[xfer_index];

        switch (xfer_index) {
        case COMMIT_XFER_VDP2_REG_TVMD:
                xfer->len = 2;
                xfer->dst = VDP2(0x0000);
                xfer->src = CPU_CACHE_THROUGH | (uint32_t)&_state_vdp2()->regs->tvmd;
                break;
        case COMMIT_XFER_VDP2_REGS:
                /* Skip committing the first 7 VDP2 registers:
                 * 0x0000 TVMD
                 * 0x0002 EXTEN
                 * 0x0004 TVSTAT R/O
                 * 0x0006 VRSIZE R/W
                 * 0x0008 HCNT   R/O
                 * 0x000A VCNT   R/O
                 * 0x000C Reserved
                 * 0x000E RAMCTL */
                xfer->len = sizeof(vdp2_registers_t) - 14;
                xfer->dst = VDP2(0x000E);
                xfer->src = CPU_CACHE_THROUGH | (uint32_t)&_state_vdp2()->regs->buffer[7];
                break;
        case COMMIT_XFER_BACK_SCREEN:
                xfer->len = _state_vdp2()->back.count * sizeof(color_rgb1555_t);
                xfer->dst = (uint32_t)_state_vdp2()->back.vram;
                xfer->src = SCU_DMA_INDIRECT_TABLE_END |
                            CPU_CACHE_THROUGH |
                            (uint32_t)_state_vdp2()->back.buffer;
                break;
        }
}

void
__vdp2_commit(scu_dma_level_t level)
{
        cpu_cache_area_purge(_state_vdp2()->regs->buffer, sizeof(vdp2_registers_t));

        const scu_dma_handle_t * const dma_handle =
            _state_vdp2()->commit.dma_handle;

        scu_dma_config_set(level, SCU_DMA_START_FACTOR_ENABLE, dma_handle, NULL);
        scu_dma_level_end_set(level, NULL, NULL);

        scu_dma_level_fast_start(level);
}

void
__vdp2_commit_wait(scu_dma_level_t level)
{
        scu_dma_level_wait(level);
}

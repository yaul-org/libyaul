/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "vdp-internal.h"

#include <cpu/cache.h>

struct state_vdp1 _internal_state_vdp1;
struct state_vdp2 _internal_state_vdp2;

static scu_dma_handle_t _commit_handle;
static scu_dma_xfer_t _commit_xfer_table[COMMIT_XFER_COUNT] __aligned(COMMIT_XFER_TABLE_ALIGNMENT);

void
_internal_vdp2_xfer_table_init(void)
{
        _state_vdp2()->commit.dma_handle = &_commit_handle;
        _state_vdp2()->commit.xfer_table = &_commit_xfer_table[0];

        _internal_vdp2_xfer_table_update(COMMIT_XFER_VDP2_REG_TVMD);
        _internal_vdp2_xfer_table_update(COMMIT_XFER_VDP2_REGS);
        _internal_vdp2_xfer_table_update(COMMIT_XFER_BACK_SCREEN);

        scu_dma_xfer_t *xfer_table;
        xfer_table = &_state_vdp2()->commit.xfer_table[0];

        scu_dma_level_cfg_t dma_cfg = {
                .mode = SCU_DMA_MODE_INDIRECT,
                .xfer.indirect = xfer_table,
                .stride = SCU_DMA_STRIDE_2_BYTES,
                .update = SCU_DMA_UPDATE_NONE
        };

        scu_dma_handle_t * const dma_handle =
            _state_vdp2()->commit.dma_handle;

        scu_dma_config_buffer(dma_handle, &dma_cfg);
}

void
_internal_vdp2_xfer_table_update(uint32_t xfer_index)
{
        scu_dma_xfer_t *xfer;
        xfer = &_state_vdp2()->commit.xfer_table[xfer_index];

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

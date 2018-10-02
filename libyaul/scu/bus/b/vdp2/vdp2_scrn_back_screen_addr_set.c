/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <cpu/cache.h>

#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_scrn_back_screen_addr_set(uint32_t vram, bool single_color)
{
        uint16_t bkclmd;
        bkclmd = (single_color) ? 0x0000 : 0x8000;

        _internal_state_vdp2.back.vram = vram;

        _internal_state_vdp2.buffered_regs.bktau = bkclmd | VRAM_BANK_4MBIT(vram);
        _internal_state_vdp2.buffered_regs.bktal = (vram >> 1) & 0xFFFF;

        /* If BDCLMD (0x0100 is set and DISP has never been set, the
         * back screen will not display properly */
        _internal_state_vdp2.buffered_regs.tvmd &= 0xFEFF;
        _internal_state_vdp2.buffered_regs.tvmd |= 0x8100;

        struct dma_xfer *dma_xfer_tbl;
        dma_xfer_tbl = &_internal_state_vdp2.commit_dma_xfer_tbl[0];

        /* If set to single color, transfer only one color
         * value. Otherwise, transfer a color buffer */
        if (single_color) {
                dma_xfer_tbl[2].len = 1 * sizeof(color_rgb555_t);
                dma_xfer_tbl[2].src = DMA_INDIRECT_TBL_END | CPU_CACHE_THROUGH |
                    (uint32_t)&_internal_state_vdp2.back.color;
        } else {
                dma_xfer_tbl[2].len = _internal_state_vdp2.back.count * sizeof(color_rgb555_t);
                dma_xfer_tbl[2].src = DMA_INDIRECT_TBL_END | CPU_CACHE_THROUGH |
                    (uint32_t)_internal_state_vdp2.back.buffer;
        }

        dma_xfer_tbl[2].dst = vram;
}

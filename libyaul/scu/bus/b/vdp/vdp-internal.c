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


/*                 xfer->len = _state_vdp2()->lncl.count * sizeof(uint16_t); */
/*                 xfer->dst = (uint32_t)_state_vdp2()->lncl.vram; */
/*                 xfer->src = CPU_CACHE_THROUGH | */
/*                             (uint32_t)_state_vdp2()->lncl.buffer; */
/*                 break; */
/*         case COMMIT_XFER_BACK_SCREEN: */
/*                 xfer->len = _state_vdp2()->back.count * sizeof(color_rgb1555_t); */
/*                 xfer->dst = (uint32_t)_state_vdp2()->back.vram; */
/*                 xfer->src = SCU_DMA_INDIRECT_TABLE_END | */
/*                             CPU_CACHE_THROUGH | */
/*                             (uint32_t)_state_vdp2()->back.buffer; */

void
__vdp2_commit(scu_dma_level_t level)
{
        vdp2_registers_t * const vdp2_regs = _state_vdp2()->regs;

        cpu_cache_area_purge(vdp2_regs->buffer, sizeof(vdp2_registers_t));
        scu_dma_level_end_set(level, NULL, NULL);
        scu_dma_transfer(level, (void *)VDP2(0x0000), vdp2_regs->buffer, sizeof(vdp2_registers_t));
}

void
__vdp2_commit_wait(scu_dma_level_t level)
{
        scu_dma_level_wait(level);
}

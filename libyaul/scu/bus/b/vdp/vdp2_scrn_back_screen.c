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

#include "vdp-internal.h"

static inline void _set_back_screen(uint32_t, const color_rgb555_t *, bool, uint16_t);

void
vdp2_scrn_back_screen_color_set(uint32_t vram, color_rgb555_t color)
{
        static color_rgb555_t buffer = COLOR_RGB555_INITIALIZER(0, 0, 0);

        assert(vram != 0x00000000);

        buffer = color;

        _set_back_screen(vram, &buffer, true, 1);
}

void
vdp2_scrn_back_screen_buffer_set(uint32_t vram, const color_rgb555_t *buffer, uint16_t count)
{
        assert(vram != 0x00000000);

        assert(buffer != NULL);

        assert(count > 0);

        _set_back_screen(vram, buffer, false, count);
}

static inline void
_set_back_screen(uint32_t vram, const color_rgb555_t *buffer, bool single_color,
    uint16_t count)
{
        uint16_t bkclmd;

        /* If set to single color, transfer only one color
         * value. Otherwise, transfer a color buffer */
        if (single_color) {
                bkclmd = 0x0000;
                count = 1;
        } else {
                bkclmd = 0x8000;
        }

        _state_vdp2()->regs.bktau = bkclmd | VRAM_BANK_4MBIT(vram);
        _state_vdp2()->regs.bktal = (vram >> 1) & 0xFFFF;

        /* If BDCLMD (0x0100 is set and DISP has never been set, the
         * back screen will not display properly */
        _state_vdp2()->regs.tvmd &= 0x7EFF;
        _state_vdp2()->regs.tvmd |= 0x8100;

        struct dma_xfer *xfer;
        xfer = &_state_vdp2()->commit.xfer_table[COMMIT_XFER_BACK_SCREEN_BUFFER];

        xfer->len = count * sizeof(color_rgb555_t);
        xfer->dst = vram;
        xfer->src = DMA_INDIRECT_TBL_END | CPU_CACHE_THROUGH | (uint32_t)buffer;
}

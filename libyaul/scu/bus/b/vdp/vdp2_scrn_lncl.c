/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <cpu/cache.h>

#include <vdp2/vram.h>

#include "vdp-internal.h"

static void _lncl_set(vdp2_vram_t vram,
    const uint16_t *buffer, const uint16_t count);

void
vdp2_scrn_lncl_color_set(vdp2_vram_t vram, const uint16_t cram_index)
{
        static uint16_t buffered_index = 0;

        buffered_index = cram_index;

        _lncl_set(vram, &buffered_index, 1);
}

void
vdp2_scrn_lncl_buffer_set(vdp2_vram_t vram, const uint16_t *buffer,
    const uint16_t count)
{
#ifdef DEBUG
        assert(buffer != NULL);
        assert(count > 0);
#endif /* DEBUG */

        _lncl_set(vram, buffer, count);
}

void
vdp2_scrn_lncl_set(vdp2_scrn_t scroll_screen)
{
        vdp2_registers_t * const vdp2_regs = _state_vdp2()->regs;

        vdp2_regs->lnclen &= scroll_screen;
}

void
vdp2_scrn_lncl_clear(void)
{
        _state_vdp2()->regs->lnclen = 0x0000;
}

static void
_lncl_set(vdp2_vram_t vram, const uint16_t *buffer, const uint16_t count)
{
        /* If set to single color, transfer only one color value. Otherwise,
         * transfer a color buffer */
        const uint16_t lcclmd = (count == 1) ? 0x0000 : 0x8000;

        _state_vdp2()->regs->lctau = lcclmd | VDP2_VRAM_BANK(vram);
        _state_vdp2()->regs->lctal = (vram >> 1) & 0xFFFF;

        _state_vdp2()->lncl.vram = vram;
        _state_vdp2()->lncl.buffer = (uintptr_t)buffer;
        _state_vdp2()->lncl.count = count;

        __vdp2_xfer_table_update(COMMIT_XFER_LNCL_SCREEN);
}

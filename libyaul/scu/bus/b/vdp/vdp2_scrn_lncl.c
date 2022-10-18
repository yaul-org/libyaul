/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp.h>

#include "vdp-internal.h"

static void _lncl_set(vdp2_vram_t vram,
    const uint16_t *buffer, const uint32_t count);

void
vdp2_scrn_lncl_set(vdp2_scrn_t scrn_mask)
{
        _state_vdp2()->regs->lnclen &= 0xFFC0;
        _state_vdp2()->regs->lnclen |= scrn_mask;
}

void
vdp2_scrn_lncl_unset(vdp2_scrn_t scrn_mask)
{
        _state_vdp2()->regs->lnclen &= ~scrn_mask;
}

void
vdp2_scrn_lncl_clear(void)
{
        _state_vdp2()->regs->lnclen = 0x0000;
}

void
vdp2_scrn_lncl_color_set(vdp2_vram_t vram, const uint16_t cram_index)
{
        static uint16_t buffer = 0;

        buffer = cram_index;

        _lncl_set(vram, &buffer, 1);
}

void
vdp2_scrn_lncl_buffer_set(vdp2_vram_t vram, const uint16_t *buffer,
    const uint32_t count)
{
#ifdef DEBUG
        assert(buffer != NULL);
        assert(count > 0);
#endif /* DEBUG */

        _lncl_set(vram, buffer, count);
}

void
vdp2_scrn_lncl_sync(void)
{
        const size_t len = _state_vdp2()->lncl.len;

        if (len == 0) {
                return;
        }

        vdp_dma_enqueue((void *)_state_vdp2()->lncl.vram,
            _state_vdp2()->lncl.buffer,
            len);
}

static void
_lncl_set(vdp2_vram_t vram, const uint16_t *buffer, const uint32_t count)
{
        const uint16_t lcclmd = (count == 1) ? 0x0000 : 0x8000;

        _state_vdp2()->regs->lctau = lcclmd | VDP2_VRAM_BANK(vram);
        _state_vdp2()->regs->lctal = (vram >> 1) & 0xFFFF;

        if (count == 1) {
                MEMORY_WRITE(16, vram, buffer[0]);
        } else {
                _state_vdp2()->lncl.vram = vram;
                _state_vdp2()->lncl.buffer = buffer;
                _state_vdp2()->lncl.len = count * sizeof(uint16_t);
        }
}

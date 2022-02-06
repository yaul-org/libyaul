/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <cpu/cache.h>

#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

static inline void _back_screen_set(vdp2_vram_t vram,
    const color_rgb1555_t *buffer, const uint16_t count);

void
vdp2_scrn_back_screen_color_set(vdp2_vram_t vram, const color_rgb1555_t color)
{
        static color_rgb1555_t buffered_color =
            COLOR_RGB1555_INITIALIZER(1, 0, 0, 0);

        buffered_color = color;

        _back_screen_set(vram, &buffered_color, 1);
}

void
vdp2_scrn_back_screen_buffer_set(vdp2_vram_t vram, const color_rgb1555_t *buffer,
    const uint16_t count)
{
#ifdef DEBUG
        assert(buffer != NULL);
        assert(count > 0);
#endif /* DEBUG */

        _back_screen_set(vram, buffer, count);
}

static inline void
_back_screen_set(vdp2_vram_t vram, const color_rgb1555_t *buffer, const uint16_t count)
{
        /* If set to single color, transfer only one color value. Otherwise,
         * transfer a color buffer */
        const uint16_t bkclmd = (count == 1) ? 0x0000 : 0x8000;

        _state_vdp2()->regs->bktau = bkclmd | VDP2_VRAM_BANK(vram);
        _state_vdp2()->regs->bktal = (vram >> 1) & 0xFFFF;

        _state_vdp2()->back.vram = vram;
        _state_vdp2()->back.buffer = (uintptr_t)buffer;
        _state_vdp2()->back.count = count;

        __vdp2_xfer_table_update(COMMIT_XFER_BACK_SCREEN);
}

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

static inline void _set_back_screen(uint32_t, const color_rgb555_t *, bool, uint16_t);

void
vdp2_scrn_back_screen_color_set(uint32_t vram, color_rgb555_t color)
{
        static color_rgb555_t buffer = COLOR_RGB1555(1, 0, 0, 0);

#ifdef DEBUG
        assert(vram != 0x00000000);
#endif /* DEBUG */

        buffer = color;

        _set_back_screen(vram, &buffer, true, 1);
}

void
vdp2_scrn_back_screen_buffer_set(uint32_t vram, const color_rgb555_t *buffer,
    uint16_t count)
{
#ifdef DEBUG
        assert(vram != 0x00000000);

        assert(buffer != NULL);

        assert(count > 0);
#endif /* DEBUG */

        bool single_color;
        single_color = (count == 1);

        _set_back_screen(vram, buffer, single_color, count);
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

        _state_vdp2()->regs.bktau = bkclmd | VDP2_VRAM_BANK(vram);
        _state_vdp2()->regs.bktal = (vram >> 1) & 0xFFFF;

        /* If BDCLMD (0x0100 is set and DISP has never been set, the
         * back screen will not display properly */
        _state_vdp2()->regs.tvmd &= 0x7EFF;
        _state_vdp2()->regs.tvmd |= 0x8100;

        _state_vdp2()->back.vram = (void *)vram;
        _state_vdp2()->back.buffer = (void *)buffer;
        _state_vdp2()->back.count = count;
}

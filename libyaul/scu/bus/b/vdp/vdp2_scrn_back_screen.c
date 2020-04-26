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

static inline void _back_screen_set(uint32_t vram, const color_rgb555_t *buffer, const uint16_t count);

void
vdp2_scrn_back_screen_color_set(uint32_t vram, const color_rgb555_t color)
{
        static color_rgb555_t buffer = COLOR_RGB1555(1, 0, 0, 0);

#ifdef DEBUG
        assert(vram != 0x00000000);
#endif /* DEBUG */

        buffer = color;

        _back_screen_set(vram, &buffer, 1);
}

void
vdp2_scrn_back_screen_buffer_set(uint32_t vram, const color_rgb555_t *buffer,
    const uint16_t count)
{
#ifdef DEBUG
        assert(vram != 0x00000000);

        assert(buffer != NULL);

        assert(count > 0);
#endif /* DEBUG */

        _back_screen_set(vram, buffer, count);
}

static inline void
_back_screen_set(uint32_t vram, const color_rgb555_t *buffer, const uint16_t count)
{
        /* If set to single color, transfer only one color value. Otherwise,
         * transfer a color buffer */
        const uint16_t bkclmd = (count == 1) ? 0x0000 : 0x8000;

        _state_vdp2()->regs->bktau = bkclmd | VDP2_VRAM_BANK(vram);
        _state_vdp2()->regs->bktal = (vram >> 1) & 0xFFFF;

        /* If BDCLMD (0x0100 is set and DISP has never been set, the back screen
         * will not display properly */
        _state_vdp2()->regs->tvmd &= 0x7EFF;
        _state_vdp2()->regs->tvmd |= 0x8100;

        _state_vdp2()->back.vram = (void *)vram;
        _state_vdp2()->back.buffer = (void *)buffer;
        _state_vdp2()->back.count = count;
}

/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_scrn_back_screen_addr_set(uint32_t vram, bool single_color)
{
        uint16_t bkclmd;
        bkclmd = (single_color) ? 0x0000 : 0x8000;

        vdp2_state.back.vram = vram;

        vdp2_state.buffered_regs.bktau = bkclmd | VRAM_BANK_4MBIT(vram);
        vdp2_state.buffered_regs.bktal = (vram >> 1) & 0xFFFF;

        vdp2_state.buffered_regs.tvmd &= 0xFEFF;

        /* Force display.
         *
         * If BDCLMD (0x0100 is set and DISP has never been set, the
         * back screen will not display properly. */
        vdp2_state.buffered_regs.tvmd |= 0x8100;

        /* Write to TVMD bit during VBLANK-IN */
        vdp2_tvmd_vblank_in_wait();

        MEMORY_WRITE(16, VDP2(TVMD), vdp2_state.buffered_regs.tvmd);
}

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
vdp2_scrn_back_screen_addr_set(bool single_color, uint32_t vram)
{
        /* Wait for the start of VBLANK-IN */
        vdp2_tvmd_vblank_in_wait();

        MEMORY_WRITE(16, VDP2(BKTAU),
            (single_color ? 0x0000 : 0x8000) | ((vram >> 17) & 0x03));
        MEMORY_WRITE(16, VDP2(BKTAL), (vram >> 1) & 0xFFFF);


        uint16_t tvmd;
        tvmd = MEMORY_READ(16, VDP2(TVMD));
        tvmd &= 0xFEFF;
        /* Force display. If BDCLMD (0x0100 is set and DISP has never
         * been set, the back screen will not display properly. */
        tvmd |= 0x8100;

        MEMORY_WRITE(16, VDP2(TVMD), tvmd);
}

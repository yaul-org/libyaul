/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_tvmd_blcs_set(bool lcclmd, uint32_t vram, uint16_t *color, uint16_t len)
{
        /* Force display. If BDCLMD is set and DISP has never been set,
         * the back screen will not display properly. */
        vdp2_tvmd_vblank_in_wait();

        MEMORY_WRITE(16, VDP2(BKTAU), (lcclmd ? 0x8000 : 0x0000) | ((vram >> 17) & 0x03));
        MEMORY_WRITE(16, VDP2(BKTAL), (vram >> 1) & 0xFFFF);

        uint16_t tvmd;

        tvmd = MEMORY_READ(16, VDP2(TVMD));
        tvmd &= 0x7EFF;
        tvmd |= 0x8100;

        MEMORY_WRITE(16, VDP2(TVMD), tvmd);

        if (!lcclmd) {
                MEMORY_WRITE(16, vram, *color);
        } else {
                uint16_t ofs;

                for (ofs = 0; ofs < len; ofs++) {
                        MEMORY_WRITE(16, vram + (ofs << 1), *color++);
                }
        }
}

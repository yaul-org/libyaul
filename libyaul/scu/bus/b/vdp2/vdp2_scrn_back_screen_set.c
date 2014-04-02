/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include <assert.h>

#include "vdp2-internal.h"

void
vdp2_scrn_back_screen_set(bool single_color, uint32_t vram, uint16_t *color,
    uint16_t len)
{

        vdp2_tvmd_vblank_in_wait();

        MEMORY_WRITE(16, VDP2(BKTAU),
            (single_color ? 0x0000 : 0x8000) | ((vram >> 17) & 0x03));
        MEMORY_WRITE(16, VDP2(BKTAL), (vram >> 1) & 0xFFFF);

        /* Force display. If BDCLMD is set and DISP has never been set,
         * the back screen will not display properly. */
        vdp2_tvmd_display_set();

        uint16_t tvmd;

        tvmd = MEMORY_READ(16, VDP2(TVMD));
        tvmd &= 0xFEFF;
        tvmd |= 0x0100;

        MEMORY_WRITE(16, VDP2(TVMD), tvmd);

        if (single_color) {
                MEMORY_WRITE(16, vram, *color);
        } else {
                /* Make sure that the number of lines to draw is a
                 * multiple of 4 */
                assert((len & 0x03) == 0x00);

                uint16_t idx;

                for (idx = 0; idx < (len / 4); ) {
                        MEMORY_WRITE(16, vram + ((idx++) << 1), *color++);
                        MEMORY_WRITE(16, vram + ((idx++) << 1), *color++);
                        MEMORY_WRITE(16, vram + ((idx++) << 1), *color++);
                        MEMORY_WRITE(16, vram + ((idx++) << 1), *color++);
                }
        }
}

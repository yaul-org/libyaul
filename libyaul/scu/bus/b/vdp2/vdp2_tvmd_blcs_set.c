/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2_internal.h"

void
vdp2_tvmd_blcs_set(bool lcclmd, uint8_t bank, uint32_t offset, uint16_t *color, uint16_t len)
{
        static uint16_t bktau = 0;
        static uint16_t bktal = 0;

        uint16_t i;

        bktau = (VRAM_BANK(bank, offset) >> 17) & 0x7;
        bktal = (VRAM_BANK(bank, offset) >> 1) & 0xFFFF;

        MEM_POKE(VDP2(BKTAU), bktau | (lcclmd ? 0x8000 : 0x0000));
        MEM_POKE(VDP2(BKTAL), bktal);

        /*
         * Force display ON. If BDCLMD is set and DISP has never been
         * set, the back screen will not display properly
         */
        MEM_POKE(VDP2(TVMD), MEM_READ(VDP2(TVMD)) | 0x8100);

        if (lcclmd) {
                for (i = 0; i < len; i++)
                        MEM_POKE(VRAM_BANK(bank, i + offset), *color++);
                return;
        }
        MEM_POKE(VRAM_BANK(bank, offset), *color);
}

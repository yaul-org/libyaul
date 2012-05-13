/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "vdp2_internal.h"

void
vdp2_tvmd_blcs_set(bool lcclmd, uint32_t vram, uint16_t *color, uint16_t len)
{
        uint16_t bktau;
        uint16_t bktal;
        uint16_t tvmd;

        uint16_t ofs;

        switch (vram_ctl.vram_size) {
        case VRAM_CTL_SIZE_4MBIT:
                bktal = VRAM_BANK_4MBIT(vram);
                bktau = VRAM_OFFSET_4MBIT(vram);
                break;
        case VRAM_CTL_SIZE_8MBIT:
                bktal = VRAM_BANK_8MBIT(vram);
                bktau = VRAM_OFFSET_8MBIT(vram);
                break;
        default:
                return;
        }

        bktau |= lcclmd ? 0x8000 : 0x0000;

        MEM_POKE(VDP2(BKTAU), bktau);
        MEM_POKE(VDP2(BKTAL), bktal);

        /*
         * Force display ON. If BDCLMD is set and DISP has never been
         * set, the back screen will not display properly
         */
        tvmd = MEM_READ(VDP2(TVMD));
        tvmd &= 0x7EFF;
        tvmd |= 0x8100;
        MEM_POKE(VDP2(TVMD), tvmd);

        if (lcclmd) {
                for (ofs = 0; ofs < len; ofs++)
                        MEM_POKE(vram + (ofs << 1), *color++);
                return;
        }

        MEM_POKE(vram, *color);
}

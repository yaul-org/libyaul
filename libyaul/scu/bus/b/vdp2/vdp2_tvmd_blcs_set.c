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

        MEMORY_WRITE(16, VDP2(BKTAU), bktau);
        MEMORY_WRITE(16, VDP2(BKTAL), bktal);

        /*
         * Force display ON. If BDCLMD is set and DISP has never been
         * set, the back screen will not display properly
         */
        tvmd = MEMORY_READ(16, VDP2(TVMD));
        tvmd &= 0x7EFF;
        tvmd |= 0x8100;
        MEMORY_WRITE(16, VDP2(TVMD), tvmd);

        if (lcclmd) {
                for (ofs = 0; ofs < len; ofs++)
                        MEMORY_WRITE(16, vram + (ofs << 1), *color++);
                return;
        }

        MEMORY_WRITE(16, vram, *color);
}

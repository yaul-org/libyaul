/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_scv_x_set(uint8_t scrn, uint16_t in, uint8_t dn)
{
        in &= 0x07FF;

        dn &= 0xFF;
        dn <<= 8;

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCXIN0), in);
                MEMORY_WRITE(16, VDP2(SCXDN0), dn);
                break;
        case SCRN_NBG1:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCXIN1), in);
                MEMORY_WRITE(16, VDP2(SCXDN1), dn);
                break;
        case SCRN_NBG2:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCXN2), in);
                break;
        case SCRN_NBG3:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCXN3), in);
                break;
        default:
                return;
        }
}

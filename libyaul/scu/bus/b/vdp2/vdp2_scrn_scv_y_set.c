/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_scv_y_set(uint8_t scrn, uint16_t in, uint8_t dn)
{
        /* Wraps around */
        in &= 0x07FF;

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                dn &= 0xFF;
                dn <<= 8;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYIN0), in);
                MEMORY_WRITE(16, VDP2(SCYDN0), dn);
                break;
        case SCRN_NBG1:
                dn &= 0xFF;
                dn <<= 8;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYIN1), in);
                MEMORY_WRITE(16, VDP2(SCYDN1), dn);
                break;
        case SCRN_NBG2:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYN2), in);
                break;
        case SCRN_NBG3:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYN3), in);
                break;
        default:
                return;
        }
}

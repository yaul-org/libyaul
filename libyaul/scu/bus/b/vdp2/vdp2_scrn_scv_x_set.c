/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

void
vdp2_scrn_scv_x_set(uint8_t scrn, uint16_t in, uint8_t dn)
{
        in &= 0x07FF;

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                dn &= 0xFF;
                dn <<= 8;

                /* Write to memory */
                MEM_POKE(VDP2(SCXIN0), in);
                MEM_POKE(VDP2(SCXDN0), dn);
                break;
        case SCRN_NBG1:
                dn &= 0xFF;
                dn <<= 8;

                /* Write to memory */
                MEM_POKE(VDP2(SCXIN1), in);
                MEM_POKE(VDP2(SCXDN1), dn);
                break;
        case SCRN_NBG2:
                /* Write to memory */
                MEM_POKE(VDP2(SCXN2), in);
                break;
        case SCRN_NBG3:
                /* Write to memory */
                MEM_POKE(VDP2(SCXN3), in);
                break;
        default:
        }
}

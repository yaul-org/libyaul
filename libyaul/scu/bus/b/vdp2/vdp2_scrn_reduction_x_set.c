/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp2-internal.h"

void
vdp2_scrn_reduction_x_set(uint8_t scrn, uint16_t in, uint16_t dn)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_NBG1));
#endif /* DEBUG */

        /* Integer part rounded to 3 bits */
        in &= 0x07;
        /* Fractional part rounded to 8 bits, shifted left by 8 */
        dn &= 0xFF;
        dn <<= 8;

        switch (scrn) {
        case SCRN_NBG0:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(ZMXIN0), in);
                MEMORY_WRITE(16, VDP2(ZMXDN0), dn);
                break;
        case SCRN_NBG1:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(ZMXIN1), in);
                MEMORY_WRITE(16, VDP2(ZMXDN1), dn);
                break;
        default:
                return;
        }
}

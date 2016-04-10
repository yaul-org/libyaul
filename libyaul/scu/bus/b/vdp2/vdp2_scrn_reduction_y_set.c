/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp2-internal.h"

/* The coord increment should be a value smaller then 1 to zoom in and
 * larger than 1 to zoom out. No zoom means equal to 1.
 *
 * Notes:
 *  1. Only NBG0 and NBG1 can be zoomed.
 *  2. Reduction can be changed during horizontal retrace.
 *  3. Max reduction out is set to 1/4 means value = 4
 *  4. Max reduction out is constrainted by bitmap color depth: 1/4 in 16
 *     colors mode, 1/2 in 16/256 colors mode. */
void
vdp2_scrn_reduction_y_set(uint8_t scrn, uint16_t in, uint16_t dn)
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
                MEMORY_WRITE(16, VDP2(ZMYIN0), in);
                MEMORY_WRITE(16, VDP2(ZMYDN0), dn);
                break;
        case SCRN_NBG1:
                /* Write to memory */
                MEMORY_WRITE(16, VDP2(ZMYIN1), in);
                MEMORY_WRITE(16, VDP2(ZMYDN1), dn);
                break;
        default:
                return;
        }
}

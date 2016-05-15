/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp2-internal.h"

const struct scrn_cell_format *
vdp2_scrn_cell_format_get(uint8_t scrn)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_RBG1) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG1));
#endif /* DEBUG */

        switch (scrn) {
        case SCRN_NBG0:
                return &vdp2_state.nbg0.cell_format;
        case SCRN_NBG1:
                return &vdp2_state.nbg1.cell_format;
        case SCRN_NBG2:
                return &vdp2_state.nbg2.cell_format;
        case SCRN_NBG3:
                return &vdp2_state.nbg3.cell_format;
        default:
                return NULL;
        }
}

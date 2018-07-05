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
vdp2_scrn_sf_codes_set(uint8_t code, uint8_t codes)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((code == SCRN_SF_CODE_A) ||
               (code == SCRN_SF_CODE_B));
#endif /* DEBUG */

        switch (code) {
        case SCRN_SF_CODE_A:
                vdp2_state.buffered_regs.sfcode &= 0xFF00;
                vdp2_state.buffered_regs.sfcode |= (uint16_t)codes;
                break;
        case SCRN_SF_CODE_B:
                vdp2_state.buffered_regs.sfcode &= 0x00FF;
                vdp2_state.buffered_regs.sfcode |= (uint16_t)codes << 8;
                break;
        }
}

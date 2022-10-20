/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_sf_codes_set(vdp2_scrn_sf_code_t code, vdp2_scrn_sf_code_range_t code_range)
{
        switch (code) {
        case VDP2_SCRN_SF_CODE_A:
                _state_vdp2()->regs->sfcode &= 0xFF00;
                _state_vdp2()->regs->sfcode |= (uint16_t)code_range;
                break;
        case VDP2_SCRN_SF_CODE_B:
                _state_vdp2()->regs->sfcode &= 0x00FF;
                _state_vdp2()->regs->sfcode |= (uint16_t)code_range << 8;
                break;
        default:
                break;
        }
}

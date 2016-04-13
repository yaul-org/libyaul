/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

uint8_t
vdp2_sprite_type_priority_get(uint8_t type)
{
#ifdef DEBUG
        assert((type >= 0) && (type <= 7));
#endif /* DEBUG */

        uint16_t shift;
        shift = (type & 0x01) << 3;
        uint16_t reg_mask;
        reg_mask = 0x07 << shift;

        switch (type) {
        case 0:
        case 1:
                return vdp2_state.buffered_regs.prisa & mask;
        case 2:
        case 3:
                return vdp2_state.buffered_regs.prisb & mask;
        case 4:
        case 5:
                return vdp2_state.buffered_regs.prisc & mask;
        case 6:
        case 7:
                return vdp2_state.buffered_regs.prisd & mask;
        default:
                return 0;
        }
}

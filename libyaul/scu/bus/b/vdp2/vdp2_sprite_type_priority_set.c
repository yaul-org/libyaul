/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_sprite_type_priority_set(uint8_t type, uint8_t priority)
{
#ifdef DEBUG
        assert((type >= 0) && (type <= 7));
#endif /* DEBUG */

        /* When priority is zero, scroll screen is transparent. */
        priority &= 0x07;

        uint16_t shift;
        shift = (type & 0x01) << 3;
        uint16_t reg_mask;
        reg_mask = ~(0x07 << shift);

        switch (type) {
        case 0:
        case 1:
                vdp2_state.buffered_regs.prisa &= reg_mask;
                vdp2_state.buffered_regs.prisa |= priority << shift;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(PRISA), vdp2_state.buffered_regs.prisa);
                break;
        case 2:
        case 3:
                vdp2_state.buffered_regs.prisb &= reg_mask;
                vdp2_state.buffered_regs.prisb |= priority << shift;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(PRISB), vdp2_state.buffered_regs.prisb);
                break;
        case 4:
        case 5:
                vdp2_state.buffered_regs.prisc &= reg_mask;
                vdp2_state.buffered_regs.prisc |= priority << shift;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(PRISC), vdp2_state.buffered_regs.prisc);
                break;
        case 6:
        case 7:
                vdp2_state.buffered_regs.prisd &= reg_mask;
                vdp2_state.buffered_regs.prisd |= priority << shift;

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(PRISD), vdp2_state.buffered_regs.prisd);
                break;
        default:
                return;
        }
}

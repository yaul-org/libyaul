/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_sprite_priority_set(uint8_t sprite_register, uint8_t priority)
{
#ifdef DEBUG
        assert((sprite_register >= 0) && (sprite_register <= 7));
#endif /* DEBUG */

        /* When priority is zero, scroll screen is transparent. */
        priority &= 0x07;

        uint16_t shift;
        shift = (sprite_register & 0x01) << 3;

        uint16_t reg_mask;
        reg_mask = 0xFFFF & ~(0x07 << shift);

        switch (sprite_register) {
        case 0:
        case 1:
                vdp2_state.buffered_regs.prisa &= reg_mask;
                vdp2_state.buffered_regs.prisa |= priority << shift;
                break;
        case 2:
        case 3:
                vdp2_state.buffered_regs.prisb &= reg_mask;
                vdp2_state.buffered_regs.prisb |= priority << shift;
                break;
        case 4:
        case 5:
                vdp2_state.buffered_regs.prisc &= reg_mask;
                vdp2_state.buffered_regs.prisc |= priority << shift;
                break;
        case 6:
        case 7:
                vdp2_state.buffered_regs.prisd &= reg_mask;
                vdp2_state.buffered_regs.prisd |= priority << shift;
                break;
        default:
                return;
        }
}

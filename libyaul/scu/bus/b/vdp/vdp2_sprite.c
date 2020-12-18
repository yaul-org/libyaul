/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

void
vdp2_sprite_priority_set(vdp2_sprite_register_t sprite_register, uint8_t priority)
{
#ifdef DEBUG
        assert(sprite_register <= 7);
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
                _state_vdp2()->regs->prisa &= reg_mask;
                _state_vdp2()->regs->prisa |= priority << shift;
                break;
        case 2:
        case 3:
                _state_vdp2()->regs->prisb &= reg_mask;
                _state_vdp2()->regs->prisb |= priority << shift;
                break;
        case 4:
        case 5:
                _state_vdp2()->regs->prisc &= reg_mask;
                _state_vdp2()->regs->prisc |= priority << shift;
                break;
        case 6:
        case 7:
                _state_vdp2()->regs->prisd &= reg_mask;
                _state_vdp2()->regs->prisd |= priority << shift;
                break;
        default:
                break;
        }
}

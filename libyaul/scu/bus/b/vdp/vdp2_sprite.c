/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/scrn.h>

#include "vdp-internal.h"

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
                _state_vdp2()->regs.prisa &= reg_mask;
                _state_vdp2()->regs.prisa |= priority << shift;
                break;
        case 2:
        case 3:
                _state_vdp2()->regs.prisb &= reg_mask;
                _state_vdp2()->regs.prisb |= priority << shift;
                break;
        case 4:
        case 5:
                _state_vdp2()->regs.prisc &= reg_mask;
                _state_vdp2()->regs.prisc |= priority << shift;
                break;
        case 6:
        case 7:
                _state_vdp2()->regs.prisd &= reg_mask;
                _state_vdp2()->regs.prisd |= priority << shift;
                break;
        default:
                return;
        }
}

void
vdp2_sprite_type_set(uint8_t type)
{
#ifdef DEBUG
        assert((type >= 0x00) && (type <= 0x0F));

        /* XXX: Check if the sprite color format is valid with the
         * sprite type specified. */
#endif /* DEBUG */

        /* Types 0 to 7 are for low resolution (320 or 352), and types 8
         * to 15 are for high resolution (640 or 704).
         *
         * The data widths are 16-bits and 8-bits, respectively. */
        _state_vdp2()->regs.spctl &= 0xFFF0;
        _state_vdp2()->regs.spctl |= type & 0x000F;
}

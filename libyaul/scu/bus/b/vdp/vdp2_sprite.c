/*
 * Copyright (c) Israel Jacquez
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
    /* When priority is zero, scroll screen is transparent. */
    priority &= 0x07;

    switch (sprite_register) {
    case 0:
        _state_vdp2()->shadow_regs.prisa &= 0xFFF8;
        _state_vdp2()->shadow_regs.prisa |= priority;
        break;
    case 1:
        _state_vdp2()->shadow_regs.prisa &= 0xF8FF;
        _state_vdp2()->shadow_regs.prisa |= priority << 8;
        break;
    case 2:
        _state_vdp2()->shadow_regs.prisb &= 0xFFF8;
        _state_vdp2()->shadow_regs.prisb |= priority;
        break;
    case 3:
        _state_vdp2()->shadow_regs.prisb &= 0xF8FF;
        _state_vdp2()->shadow_regs.prisb |= priority << 8;
        break;
    case 4:
        _state_vdp2()->shadow_regs.prisc &= 0xFFF8;
        _state_vdp2()->shadow_regs.prisc |= priority;
        break;
    case 5:
        _state_vdp2()->shadow_regs.prisc &= 0xF8FF;
        _state_vdp2()->shadow_regs.prisc |= priority << 8;
        break;
    case 6:
        _state_vdp2()->shadow_regs.prisd &= 0xFFF8;
        _state_vdp2()->shadow_regs.prisd |= priority;
        break;
    case 7:
        _state_vdp2()->shadow_regs.prisd &= 0xF8FF;
        _state_vdp2()->shadow_regs.prisd |= priority << 8;
        break;
    default:
        break;
    }
}

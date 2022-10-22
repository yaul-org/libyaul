/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/cram.h>

#include "vdp-internal.h"

vdp2_cram_mode_t
vdp2_cram_mode_get(void)
{
        return ((_state_vdp2()->regs->ramctl >> 12) & 0x03);
}

void
vdp2_cram_mode_set(vdp2_cram_mode_t mode)
{
        _state_vdp2()->regs->ramctl &= 0xCFFF;
        _state_vdp2()->regs->ramctl |= mode << 12;

        MEMORY_WRITE(16, VDP2(RAMCTL), _state_vdp2()->regs->ramctl);
}

void
vdp2_cram_offset_set(vdp2_scrn_t scroll_screen, vdp2_cram_t cram)
{
        cram = (((vdp2_cram_mode_get()) == 2)
            ? (cram >> 10)
            : ((cram >> 9)) & 0x07);

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs->craofa &= 0xFFF8;
                _state_vdp2()->regs->craofa |= cram;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->craofa &= 0xFF8F;
                _state_vdp2()->regs->craofa |= cram << 4;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->regs->craofa &= 0xF8FF;
                _state_vdp2()->regs->craofa |= cram << 8;
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->regs->craofa &= 0x8FFF;
                _state_vdp2()->regs->craofa |= cram << 12;
                break;
        case VDP2_SCRN_RBG0:
        case VDP2_SCRN_RBG0_PA:
        case VDP2_SCRN_RBG0_PB:
                _state_vdp2()->regs->craofb &= 0xFFF8;
                _state_vdp2()->regs->craofb |= cram;
                break;
        case VDP2_SCRN_SPRITE:
                _state_vdp2()->regs->craofb &= 0xFF8F;
                _state_vdp2()->regs->craofb |= cram << 4;
                break;
        default:
                break;
        }
}

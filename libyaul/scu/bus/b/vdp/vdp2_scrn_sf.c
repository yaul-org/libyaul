/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_sf_set(vdp2_scrn_t scroll_screen, uint32_t sf_mode,
    vdp2_scrn_sf_type_t sf_type, vdp2_scrn_sf_code_t sf_code)
{
        sf_code &= 0x01;
        sf_mode &= 0x03;

        const bool is_format_cell = ((sf_type & 0x08) == 0x00);

        sf_type &= 0xF7;

        /* Values in vdp2_scrn_sf_type_t are shifted to the left by one. Bit 3
         * signifies screen display format: cell or bitmap format */
        uint16_t sf_type_bits;
        sf_type_bits = sf_type;

        uint16_t *pnc_reg;
        pnc_reg = (is_format_cell) ? &_state_vdp2()->regs->pncn0 : NULL;

        uint16_t *bmp_reg;
        bmp_reg = (is_format_cell) ? NULL : &_state_vdp2()->regs->bmpna;

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs->sfsel &= 0xFFFE;
                _state_vdp2()->regs->sfsel |= sf_code;

                _state_vdp2()->regs->sfprmd &= 0xFFFC;
                _state_vdp2()->regs->sfprmd |= sf_mode;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->sfsel &= 0xFFFD;
                _state_vdp2()->regs->sfsel |= sf_code << 1;

                _state_vdp2()->regs->sfprmd &= 0xFFF3;
                _state_vdp2()->regs->sfprmd |= sf_mode << 2;

                if (is_format_cell) {
                        pnc_reg += 1;
                } else {
                        /* Special case. SF type bits are shifted by 8 for
                         * NBG1 */
                        sf_type_bits <<= 8;
                }
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->regs->sfsel &= 0xFFFB;
                _state_vdp2()->regs->sfsel |= sf_code << 2;

                _state_vdp2()->regs->sfprmd &= 0xFFCF;
                _state_vdp2()->regs->sfprmd |= sf_mode << 4;

                pnc_reg += 2;
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->regs->sfsel &= 0xFFF7;
                _state_vdp2()->regs->sfsel |= sf_code << 3;

                _state_vdp2()->regs->sfprmd &= 0xFF3F;
                _state_vdp2()->regs->sfprmd |= sf_mode << 6;

                pnc_reg += 3;
                break;
        case VDP2_SCRN_RBG0:
        case VDP2_SCRN_RBG0_PA:
        case VDP2_SCRN_RBG0_PB:
                _state_vdp2()->regs->sfsel &= 0xFFEF;
                _state_vdp2()->regs->sfsel |= sf_code << 4;

                _state_vdp2()->regs->sfprmd &= 0xFCFF;
                _state_vdp2()->regs->sfprmd |= sf_mode << 8;

                if (is_format_cell) {
                        bmp_reg += 1;
                } else {
                        pnc_reg += 4;
                }
                break;
        default:
                return;
        }

        if (is_format_cell) {
                sf_type_bits <<= 8;

                *pnc_reg &= 0xFCFF;
                *pnc_reg |= sf_type_bits;
        } else {
                *bmp_reg |= sf_type_bits;
        }
}

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

/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <vdp2/cram.h>
#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

void
vdp2_scrn_rotation_cell_format_set(const vdp2_scrn_cell_format_t *cell_format,
  const vdp2_scrn_rotation_map_t *rotation_map)
{
    vdp2_scrn_cell_ccc_set(cell_format);
    vdp2_scrn_char_size_set(cell_format);
    vdp2_scrn_plane_size_set(cell_format);

    if (rotation_map->single) {
        vdp2_scrn_map_plane_set(cell_format, VDP2_SCRN_PLANE_A, rotation_map->plane);
        vdp2_scrn_map_plane_set(cell_format, VDP2_SCRN_PLANE_B, rotation_map->plane);

        /* Use the MPABR[AB] register to fill/copy the remaining 7 other
         * registers */
        const uint16_t *mpab_reg;
        mpab_reg = (uint16_t *)&_state_vdp2()->shadow_regs.mpabra;

        uint16_t *mp_reg;
        mp_reg = (uint16_t *)&_state_vdp2()->shadow_regs.mpcdra;

        if (cell_format->scroll_screen == VDP2_SCRN_RBG0_PB) {
            mpab_reg += 8;
            mp_reg += 8;
        }

        const uint16_t mpab_bits = *mpab_reg;

        *mp_reg++ = mpab_bits; /* CD */
        *mp_reg++ = mpab_bits; /* EF */
        *mp_reg++ = mpab_bits; /* GH */
        *mp_reg++ = mpab_bits; /* IJ */
        *mp_reg++ = mpab_bits; /* KL */
        *mp_reg++ = mpab_bits; /* MN */
        *mp_reg   = mpab_bits; /* OP */
    } else {
        for (uint32_t plane = 0; plane < 16; plane++) {
            vdp2_scrn_map_plane_set(cell_format, plane, rotation_map->base_addr[plane]);
        }
    }

    vdp2_scrn_pnd_set(cell_format);
}

void
vdp2_scrn_rotation_rp_mode_set(const vdp2_scrn_rotation_params_t *rotation_params)
{
    _state_vdp2()->shadow_regs.rpmd &= 0xFFFE;
    _state_vdp2()->shadow_regs.rpmd |= rotation_params->rp_mode;
}

void
vdp2_scrn_rotation_rp_table_set(const vdp2_scrn_rotation_params_t *rotation_params)
{
    _state_vdp2()->shadow_regs.rptau = VDP2_VRAM_BANK(rotation_params->rp_table_base);
    _state_vdp2()->shadow_regs.rptal = rotation_params->rp_table_base & 0xFFFE;
}

void
vdp2_scrn_rotation_coeff_table_set(const vdp2_scrn_rotation_params_t *rotation_params)
{
    _state_vdp2()->shadow_regs.ktaof = 0x0000;

    const vdp2_scrn_coeff_params_t * const coeff_params =
      &rotation_params->coeff_params;

    if (!coeff_params->enable) {
        return;
    }

    assert(rotation_params->rp_table != NULL);

    switch (rotation_params->coeff_params.word_size) {
    case VDP2_SCRN_COEFF_WORD_SIZE_1:
        _state_vdp2()->shadow_regs.ktaof = (rotation_params->rp_table->kast >> 17) & 0x07;
        break;
    case VDP2_SCRN_COEFF_WORD_SIZE_2:
        _state_vdp2()->shadow_regs.ktaof = (rotation_params->rp_table->kast >> 18) & 0x07;
        break;
    default:
        break;
    }
}

void
vdp2_scrn_rotation_sop_set(const vdp2_scrn_rotation_params_t *rotation_params)
{
    switch (rotation_params->rp_mode) {
    case VDP2_SCRN_RP_MODE_0:
        _state_vdp2()->shadow_regs.plsz &= 0xF3FF;
        _state_vdp2()->shadow_regs.plsz |= rotation_params->rsop_type << 10;
        break;
    case VDP2_SCRN_RP_MODE_1:
        _state_vdp2()->shadow_regs.plsz &= 0x3FFF;
        _state_vdp2()->shadow_regs.plsz |= rotation_params->rsop_type << 14;
        break;
    case VDP2_SCRN_RP_MODE_2:
        break;
    case VDP2_SCRN_RP_MODE_3:
        break;
    default:
        break;
    }
}

void
vdp2_scrn_rotation_rp_read_set(const vdp2_scrn_rotation_params_t *rotation_params)
{
    const vdp2_scrn_coeff_params_t * const coeff_params =
      &rotation_params->coeff_params;

    switch (rotation_params->rp_mode) {
    case VDP2_SCRN_RP_MODE_0:
        _state_vdp2()->shadow_regs.rprctl &= 0xFFF8;

        if (coeff_params->enable) {
            _state_vdp2()->shadow_regs.rprctl |=  coeff_params->xst_read_enable |
              (coeff_params->yst_read_enable << 1) |
              (coeff_params->kast_read_enable << 2);
        }
        break;
    case VDP2_SCRN_RP_MODE_1:
        _state_vdp2()->shadow_regs.rprctl &= 0xF8FF;

        if (coeff_params->enable) {
            _state_vdp2()->shadow_regs.rprctl |= (coeff_params->xst_read_enable << 8) |
              (coeff_params->yst_read_enable << 9) |
              (coeff_params->kast_read_enable << 10);
        }
        break;
    default:
        break;
    }
}

void
vdp2_scrn_rotation_coeff_params_set(const vdp2_scrn_rotation_params_t *rotation_params)
{
    const vdp2_scrn_coeff_params_t * const coeff_params =
      &rotation_params->coeff_params;

    switch (rotation_params->rp_mode) {
    case VDP2_SCRN_RP_MODE_0:
        _state_vdp2()->shadow_regs.ktctl &= 0xFFFF;

        if (coeff_params->enable) {
            _state_vdp2()->shadow_regs.ktctl |= (coeff_params->enable << 0) |
              (coeff_params->lncl_enable << 4) |
              (coeff_params->word_size) |
              coeff_params->usage;
        }
        break;
    case VDP2_SCRN_RP_MODE_1:
        _state_vdp2()->shadow_regs.ktctl &= 0xFFFF;

        if (coeff_params->enable) {
            _state_vdp2()->shadow_regs.ktctl |= (coeff_params->enable << 8) |
              (coeff_params->lncl_enable << 12) |
              (coeff_params->word_size << 8) |
              (coeff_params->usage << 8);
        }
        break;
    default:
        break;
    }
}

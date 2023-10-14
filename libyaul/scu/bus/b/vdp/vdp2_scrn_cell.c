/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/cram.h>
#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

void
vdp2_scrn_cell_format_set(const vdp2_scrn_cell_format_t *cell_format,
  const vdp2_scrn_normal_map_t *normal_map)
{
    vdp2_scrn_cell_ccc_set(cell_format);
    vdp2_scrn_char_size_set(cell_format);
    vdp2_scrn_plane_size_set(cell_format);
    vdp2_scrn_map_set(cell_format, 0, normal_map);
    vdp2_scrn_map_set(cell_format, 1, normal_map);
    vdp2_scrn_map_set(cell_format, 2, normal_map);
    vdp2_scrn_map_set(cell_format, 3, normal_map);
    vdp2_scrn_pnd_set(cell_format);
}

void
vdp2_scrn_cell_ccc_set(const vdp2_scrn_cell_format_t *cell_format)
{
    switch (cell_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.chctla &= 0xFF8F;
        _state_vdp2()->shadow_regs.chctla |= cell_format->ccc << 4;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.chctla &= 0xCFFF;
        _state_vdp2()->shadow_regs.chctla |= cell_format->ccc << 12;
        break;
    case VDP2_SCRN_NBG2:
        _state_vdp2()->shadow_regs.chctlb &= 0xFFFD;
        _state_vdp2()->shadow_regs.chctlb |= cell_format->ccc << 1;
        break;
    case VDP2_SCRN_NBG3:
        _state_vdp2()->shadow_regs.chctlb &= 0xFFDF;
        _state_vdp2()->shadow_regs.chctlb |= cell_format->ccc << 5;
        break;
    case VDP2_SCRN_RBG0:
    case VDP2_SCRN_RBG0_PA:
    case VDP2_SCRN_RBG0_PB:
        _state_vdp2()->shadow_regs.chctlb &= 0x8FFF;
        _state_vdp2()->shadow_regs.chctlb |= cell_format->ccc << 12;
        break;
    default:
        break;
    }
}

void
vdp2_scrn_char_size_set(const vdp2_scrn_cell_format_t *cell_format)
{
    switch (cell_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.chctla &= 0xFFFE;
        _state_vdp2()->shadow_regs.chctla |= cell_format->char_size;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.chctla &= 0xFEFF;
        _state_vdp2()->shadow_regs.chctla |= cell_format->char_size << 8;
        break;
    case VDP2_SCRN_NBG2:
        _state_vdp2()->shadow_regs.chctlb &= 0xFFFE;
        _state_vdp2()->shadow_regs.chctlb |= cell_format->char_size;
        break;
    case VDP2_SCRN_NBG3:
        _state_vdp2()->shadow_regs.chctlb &= 0xFFEF;
        _state_vdp2()->shadow_regs.chctlb |= cell_format->char_size << 4;
        break;
    case VDP2_SCRN_RBG0:
    case VDP2_SCRN_RBG0_PA:
    case VDP2_SCRN_RBG0_PB:
        _state_vdp2()->shadow_regs.chctlb &= 0xFEFF;
        _state_vdp2()->shadow_regs.chctlb |= cell_format->char_size << 8;
        break;
    default:
        break;
    }
}

void
vdp2_scrn_plane_size_set(const vdp2_scrn_cell_format_t *cell_format)
{
    const uint16_t plane_size = (cell_format->plane_size - 1) << 2;

    switch (cell_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.plsz &= 0xFFFC;
        _state_vdp2()->shadow_regs.plsz |= plane_size >> 2;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.plsz &= 0xFFF3;
        _state_vdp2()->shadow_regs.plsz |= plane_size;
        break;
    case VDP2_SCRN_NBG2:
        _state_vdp2()->shadow_regs.plsz &= 0xFFCF;
        _state_vdp2()->shadow_regs.plsz |= plane_size << 2;
        break;
    case VDP2_SCRN_NBG3:
        _state_vdp2()->shadow_regs.plsz &= 0xFF3F;
        _state_vdp2()->shadow_regs.plsz |= plane_size << 4;
        break;
    case VDP2_SCRN_RBG0:
    case VDP2_SCRN_RBG0_PA:
        _state_vdp2()->shadow_regs.plsz &= 0xFCFF;
        _state_vdp2()->shadow_regs.plsz |= plane_size << 8;
        break;
    case VDP2_SCRN_RBG0_PB:
        _state_vdp2()->shadow_regs.plsz &= 0xCFFF;
        _state_vdp2()->shadow_regs.plsz |= plane_size << 12;
        break;
    default:
        break;
    }
}

void
vdp2_scrn_map_set(const vdp2_scrn_cell_format_t *cell_format,
  vdp2_scrn_plane_t plane, const vdp2_scrn_normal_map_t *normal_map)
{
    vdp2_scrn_map_plane_set(cell_format, plane, normal_map->base_addr[plane]);
}

void
vdp2_scrn_map_plane_set(const vdp2_scrn_cell_format_t *cell_format,
  vdp2_scrn_plane_t plane, vdp2_vram_t plane_base)
{
    /* A bit less clear than just dividing the plane address by the page
     * size to get the map bits */
    uint16_t map_bits;
    map_bits = plane_base >> 11;

    const uint8_t page_config =
      (cell_format->char_size << 2) | cell_format->pnd_size;

    switch (page_config) {
    case ((VDP2_SCRN_CHAR_SIZE_2X2 << 2) | 1):
        break;
    case ((VDP2_SCRN_CHAR_SIZE_2X2 << 2) | 2):
        map_bits >>= 1;
        break;
    case ((VDP2_SCRN_CHAR_SIZE_1X1 << 2) | 1):
        map_bits >>= 2;
        break;
    case ((VDP2_SCRN_CHAR_SIZE_1X1 << 2) | 2):
        map_bits >>= 3;
        break;
    }

    map_bits &= 0x03FF;

    /* Calculate the upper 3-bits of the 9-bits "map register", but only for
     * the first plane.
     *
     * Does this then imply that the all pattern name data for all planes
     * must be stored on the same VRAM bank? */
    const uint16_t map_offset_bits = (map_bits >> 6) & 0x07;

    switch (cell_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        _state_vdp2()->shadow_regs.mpofn &= 0xFFF8;
        _state_vdp2()->shadow_regs.mpofn |= map_offset_bits;
        break;
    case VDP2_SCRN_NBG1:
        _state_vdp2()->shadow_regs.mpofn &= 0xFF8F;
        _state_vdp2()->shadow_regs.mpofn |= map_offset_bits << 4;
        break;
    case VDP2_SCRN_NBG2:
        _state_vdp2()->shadow_regs.mpofn &= 0xF8FF;
        _state_vdp2()->shadow_regs.mpofn |= map_offset_bits << 8;
        break;
    case VDP2_SCRN_NBG3:
        _state_vdp2()->shadow_regs.mpofn &= 0x8FFF;
        _state_vdp2()->shadow_regs.mpofn |= map_offset_bits << 12;
        break;
    case VDP2_SCRN_RBG0:
    case VDP2_SCRN_RBG0_PA:
        _state_vdp2()->shadow_regs.mpofr &= 0xFFF8;
        _state_vdp2()->shadow_regs.mpofr |= map_offset_bits;
        break;
    case VDP2_SCRN_RBG0_PB:
        _state_vdp2()->shadow_regs.mpofr &= 0xFF8F;
        _state_vdp2()->shadow_regs.mpofr |= map_offset_bits << 4;
        break;
    default:
        break;
    }

    uint16_t *mp_reg;
    mp_reg = (uint16_t *)&_state_vdp2()->shadow_regs.mpabn0;

    switch (cell_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        break;
    case VDP2_SCRN_NBG1:
        mp_reg += 2;
        break;
    case VDP2_SCRN_NBG2:
        mp_reg += 4;
        break;
    case VDP2_SCRN_NBG3:
        mp_reg += 6;
        break;
    case VDP2_SCRN_RBG0:
    case VDP2_SCRN_RBG0_PA:
        mp_reg += 8;
        break;
    case VDP2_SCRN_RBG0_PB:
        mp_reg += 16;
        break;
    default:
        break;
    }

    /* Choose which of the two map plane registers */
    mp_reg += (plane >> 1);

    const uint16_t plane_bits = map_bits & 0x003F;

    /* Select between first or second page within the register */
    if ((plane & 1) == 0) {
        *mp_reg &= 0xFFC0;
        *mp_reg |= plane_bits;
    } else {
        *mp_reg &= 0xC0FF;
        *mp_reg |= plane_bits << 8;
    }
}

void
vdp2_scrn_pnd_set(const vdp2_scrn_cell_format_t *cell_format)
{
    uint16_t auxiliary_mode_bits;
    auxiliary_mode_bits = 0x0000;

    uint16_t scn_bits; /* Supplementary character number bits */
    scn_bits = 0;

    uint16_t spn_bits; /* Supplementary palette number bits */
    spn_bits = 0;

    const uint16_t character_number = VDP2_SCRN_PND_CP_NUM(cell_format->cpd_base);

    uint16_t palette_number;
    palette_number = 0;

    switch ((vdp2_cram_mode_get())) {
    case 0:
        palette_number = VDP2_SCRN_PND_MODE_0_PAL_NUM(cell_format->palette_base);
        break;
    case 1:
        palette_number = VDP2_SCRN_PND_MODE_1_PAL_NUM(cell_format->palette_base);
        break;
    case 2:
        palette_number = VDP2_SCRN_PND_MODE_2_PAL_NUM(cell_format->palette_base);
        break;
    default:
        break;
    }

    uint16_t *pnc_reg;
    pnc_reg = &_state_vdp2()->shadow_regs.pncn0;

    switch (cell_format->scroll_screen) {
    case VDP2_SCRN_NBG0:
        break;
    case VDP2_SCRN_NBG1:
        pnc_reg += 1;
        break;
    case VDP2_SCRN_NBG2:
        pnc_reg += 2;
        break;
    case VDP2_SCRN_NBG3:
        pnc_reg += 3;
        break;
    case VDP2_SCRN_RBG0:
    case VDP2_SCRN_RBG0_PA:
    case VDP2_SCRN_RBG0_PB:
        pnc_reg += 4;
        break;
    default:
        break;
    }

    *pnc_reg &= 0x0300;

    switch (cell_format->pnd_size) {
    case 1: /* Pattern name data size: 1-word */

        /* Depending on the Color RAM mode, there are "invalid"
         * CRAM banks.
         *
         * Mode 0 (1024 colors, mirrored, 64 banks)
         * Mode 1 (2048 colors, 128 banks)
         * Mode 2 (1024 colors) */

        switch (cell_format->ccc) {
        case VDP2_SCRN_CCC_PALETTE_16:
            spn_bits = ((palette_number >> 4) & 0x07) << 5;
            break;
        case VDP2_SCRN_CCC_PALETTE_256:
        case VDP2_SCRN_CCC_PALETTE_2048:
        case VDP2_SCRN_CCC_RGB_32768:
        case VDP2_SCRN_CCC_RGB_16770000:
        default:
            spn_bits = 0x0000;
            break;
        }

        /* Character number supplement mode */
        switch (cell_format->aux_mode) {
        case 0:
            /* Auxiliary mode 0; flip function can be used */
            /* Supplementary character number */
            switch (cell_format->char_size) {
            case VDP2_SCRN_CHAR_SIZE_1X1:
                /* Character number in pattern name table: bits 9~0
                 * Character number in supplemental data:  bits 14 13 12 11 10 */
                scn_bits = (character_number & 0x7C00U) >> 10;
                break;
            case VDP2_SCRN_CHAR_SIZE_2X2:
                /* Character number in pattern name table: bits 11~2
                 * Character number in supplemental data:  bits 14 13 12  1  0 */
                scn_bits = ((((character_number >> 12) & 0x7) << 2) |
                  (character_number & 0x03));
                break;
            }
            break;
        case 1:
            /* Auxiliary mode 1; flip function cannot be used */
            /* Supplementary character number */
            switch (cell_format->char_size) {
            case VDP2_SCRN_CHAR_SIZE_1X1:
                /* Character number in pattern name table: bits 11~0
                 * Character number in supplemental data:  bits 14 13 12 __ __ */
                scn_bits = (character_number & 0x7000U) >> 10;
                break;
            case VDP2_SCRN_CHAR_SIZE_2X2:
                /* Character number in pattern name table: bits 13~2
                 * Character number in supplemental data:  bits 14 __ __  1  0 */
                scn_bits = ((((character_number >> 12) & 0x7) << 2) |
                  (character_number & 0x03));
                break;
            }

            auxiliary_mode_bits = 0x4000;
        }

        *pnc_reg |= 0x8000 | auxiliary_mode_bits | scn_bits | spn_bits;
        break;
    case 2: /* Pattern name data size: 2-words */
        break;
    }
}

/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include <vdp2/cram.h>
#include <vdp2/vram.h>

#include "vdp-internal.h"

static vdp2_vram_ctl_t _vdp2_vram_ctl = {
        .coefficient_table = VDP2_VRAM_CTL_COEFFICIENT_TABLE_VRAM,
        .vram_mode = VDP2_VRAM_CTL_MODE_PART_BANK_BOTH
};

void
__vdp2_vram_init(void)
{
        _state_vdp2()->vram_ctl = &_vdp2_vram_ctl;

        vdp2_vram_control_set(&_vdp2_vram_ctl);
        vdp2_vram_cycp_clear();
}

void
vdp2_vram_control_set(const vdp2_vram_ctl_t *vram_ctl)
{
#ifdef DEBUG
        assert(vram_ctl != NULL);

        assert((vram_ctl->vram_mode & 0xFC) == 0x00);

        assert((vram_ctl->coefficient_table == VDP2_VRAM_CTL_COEFFICIENT_TABLE_VRAM) ||
              ((vram_ctl->coefficient_table == VDP2_VRAM_CTL_COEFFICIENT_TABLE_CRAM)));
#endif /* DEBUG */

        /* If the coefficient table is set to be stored in CRAM, the
         * color mode must be 1 */
        _state_vdp2()->regs->ramctl &= 0xEFFF;
        _state_vdp2()->regs->ramctl |= 1 << vram_ctl->coefficient_table;

        /* Coefficient table storage */
        _state_vdp2()->regs->ramctl &= 0x7FFF;
        _state_vdp2()->regs->ramctl |= vram_ctl->coefficient_table << 15;

        /* VRAM mode */
        _state_vdp2()->regs->ramctl &= 0xFCFF;
        _state_vdp2()->regs->ramctl |= vram_ctl->vram_mode << 8;

        MEMORY_WRITE(16, VDP2(VRSIZE), 0x0000);
        MEMORY_WRITE(16, VDP2(RAMCTL), _state_vdp2()->regs->ramctl);

        (void)memcpy(_state_vdp2()->vram_ctl, &vram_ctl,
            sizeof(vdp2_vram_ctl_t));
}

void
vdp2_vram_cycp_set(const vdp2_vram_cycp_t *vram_cycp)
{
#ifdef DEBUG
        assert(vram_cycp != NULL);
#endif /* DEBUG */

        vdp2_vram_cycp_bank_set(VDP2_VRAM_BANK_A0, &vram_cycp->pt[0]);
        vdp2_vram_cycp_bank_set(VDP2_VRAM_BANK_A1, &vram_cycp->pt[1]);
        vdp2_vram_cycp_bank_set(VDP2_VRAM_BANK_B0, &vram_cycp->pt[2]);
        vdp2_vram_cycp_bank_set(VDP2_VRAM_BANK_B1, &vram_cycp->pt[3]);
}

void
vdp2_vram_cycp_clear(void)
{
        vdp2_vram_cycp_bank_clear(VDP2_VRAM_BANK_A0);
        vdp2_vram_cycp_bank_clear(VDP2_VRAM_BANK_A1);
        vdp2_vram_cycp_bank_clear(VDP2_VRAM_BANK_B0);
        vdp2_vram_cycp_bank_clear(VDP2_VRAM_BANK_B1);
}

vdp2_vram_cycp_bank_t
vdp2_vram_cycp_bank_get(vdp2_vram_bank_t bank)
{
#ifdef DEBUG
        assert(bank <= 3);
#endif /* DEBUG */

        return *(vdp2_vram_cycp_bank_t *)&_state_vdp2()->regs->cyc[bank];
}

void
vdp2_vram_cycp_bank_set(vdp2_vram_bank_t bank,
    const vdp2_vram_cycp_bank_t *cycp_bank)
{
#ifdef DEBUG
        assert(cycp_bank != NULL);
#endif /* DEBUG */

        _state_vdp2()->regs->cyc[bank] = cycp_bank->raw;
}

void
vdp2_vram_cycp_bank_clear(vdp2_vram_bank_t bank)
{
        _state_vdp2()->regs->cyc[bank] = 0xFFFFFFFF;
}

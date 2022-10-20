/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_display_set(vdp2_scrn_disp_t disp_mask)
{
        _state_vdp2()->regs->bgon |= disp_mask;
}

void
vdp2_scrn_display_unset(vdp2_scrn_disp_t disp_mask)
{
        _state_vdp2()->regs->bgon &= ~disp_mask;
}

void
vdp2_scrn_display_clear(void)
{
        _state_vdp2()->regs->bgon = 0x0000;
}

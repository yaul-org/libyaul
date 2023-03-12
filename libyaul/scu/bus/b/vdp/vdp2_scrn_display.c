/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

vdp2_scrn_disp_t
vdp2_scrn_display_get(void)
{
    return _state_vdp2()->shadow_regs.bgon;
}

void
vdp2_scrn_display_set(vdp2_scrn_disp_t disp_mask)
{
    _state_vdp2()->shadow_regs.bgon = disp_mask;
}

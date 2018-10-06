/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp-internal.h"

void
vdp2_scrn_color_offset_clear(void)
{
        _state_vdp2()->regs.clofen = 0x0000;
        _state_vdp2()->regs.clofsl = 0x0000;
}

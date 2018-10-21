/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_display_clear(void)
{
        _state_vdp2()->regs.bgon = 0x0000;
}

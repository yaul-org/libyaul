/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_rp_mode_set(enum scrn_rp_mode_type mode)
{
        vdp2_regs.rpmd &= 0xFFFE;
        vdp2_regs.rpmd |= mode;

        /* Write to memory. */
        MEMORY_WRITE(16, VDP2(RPMD), vdp2_regs.rpmd);
}

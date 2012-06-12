/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/scrn.h>

#include "vdp2_internal.h"

void
vdp2_scrn_rp_mode_set(enum scrn_rp_mode_type mode)
{
        vdp2_regs.rpmd &= 0xFFFE;
        vdp2_regs.rpmd |= mode;

        /* Write to memory. */
        MEM_POKE(VDP2(RPMD), vdp2_regs.rpmd);
}

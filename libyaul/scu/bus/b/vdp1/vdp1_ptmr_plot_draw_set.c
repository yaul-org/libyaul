/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp1/ptmr.h>

#include "vdp1_internal.h"

void
vdp1_ptmr_plot_draw_set(void)
{
        /* Start drawing with automatic frame change. */
        MEM_POKE(VDP1(PTMR), 0x0002);
}

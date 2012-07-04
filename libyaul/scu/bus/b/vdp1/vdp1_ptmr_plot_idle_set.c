/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp1/ptmr.h>

#include "vdp1-internal.h"

void
vdp1_ptmr_plot_idle_set(void)
{
        /* Stop drawing. */
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
}

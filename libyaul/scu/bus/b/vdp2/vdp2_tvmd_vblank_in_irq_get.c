/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

irq_mux_t vdp2_vblank_in_irq_mux;

irq_mux_t *
vdp2_tvmd_vblank_in_irq_get(void)
{
        /* Getter */
        return &vdp2_vblank_in_irq_mux;
}

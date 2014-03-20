/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

irq_mux_t vdp2_hblank_in_irq_mux;

irq_mux_t *
vdp2_tvmd_hblank_in_irq_get(void)
{
        /* Getter */
        return &vdp2_hblank_in_irq_mux;
}

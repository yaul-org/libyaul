/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <irq-mux.h>

irq_mux_t *
vdp2_tvmd_vblank_in_irq_get(void)
{
        extern irq_mux_t vdp2_vblank_in_irq_mux;

        /* Getter */
        return &vdp2_vblank_in_irq_mux;
}

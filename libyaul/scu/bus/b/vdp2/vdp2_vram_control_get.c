/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/vram.h>

#include "vdp2-internal.h"

struct vram_ctl *
vdp2_vram_control_get(void)
{

        return &vram_ctl;
}

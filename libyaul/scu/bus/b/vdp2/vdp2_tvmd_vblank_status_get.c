/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2_internal.h"

bool
vdp2_tvmd_vblank_status_get(void)
{
        return (MEM_READ(VDP2(TVSTAT)) & 0x8) >> 3;
}

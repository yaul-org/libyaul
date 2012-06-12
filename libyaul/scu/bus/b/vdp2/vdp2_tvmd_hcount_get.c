/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2_internal.h"

uint16_t
vdp2_tvmd_hcount_get(void)
{

        for (; ((MEM_READ(VDP2(EXTEN)) & 0x0200) == 0x0200); );

        return MEM_READ(VDP2(HCNT));
}

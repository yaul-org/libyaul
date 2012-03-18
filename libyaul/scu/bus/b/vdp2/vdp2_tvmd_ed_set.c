/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2_internal.h"

void
vdp2_tvmd_ed_set(void)
{
	MEM_POKE(VDP2(TVMD), MEM_READ(VDP2(TVMD)) | 0x8000);
}

/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

uint16_t
vdp2_tvmd_vcount_get(void)
{

        for (; ((MEMORY_READ(16, VDP2(EXTEN)) & 0x0200) == 0x0200); );

        return MEMORY_READ(16, VDP2(VCNT));
}

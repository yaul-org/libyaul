/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_vram_control_set(struct vram_ctl *ctl)
{
        uint16_t vrsize;
        uint16_t ramctl;

        /* VRAM size */
        vrsize = ctl->vram_size;
        MEMORY_WRITE(16, VDP2(VRSIZE), vrsize);

        /* VRAM mode */
        ramctl = MEMORY_READ(16, VDP2(RAMCTL));
        ramctl &= 0xFCFF;
        ramctl |= ctl->vram_mode;

        MEMORY_WRITE(16, VDP2(RAMCTL), ramctl);

        /* Cycle patterns */
        /* The VDP2 is not smart enough to direct all of this VRAM
         * traffic automatically; you must do it yourself */
        MEMORY_WRITE(16, VDP2(CYCA0L), ctl->vram_cycp.pv[0] & 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCA0U), (ctl->vram_cycp.pv[0] >> 16) & 0xFFFF);

        MEMORY_WRITE(16, VDP2(CYCA1L), ctl->vram_cycp.pv[1] & 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCA1U), (ctl->vram_cycp.pv[1] >> 16) & 0xFFFF);

        MEMORY_WRITE(16, VDP2(CYCB0L), ctl->vram_cycp.pv[2] & 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCB0U), (ctl->vram_cycp.pv[2] >> 16) & 0xFFFF);

        MEMORY_WRITE(16, VDP2(CYCB1L), ctl->vram_cycp.pv[3] & 0xFFFF);
        MEMORY_WRITE(16, VDP2(CYCB1U), (ctl->vram_cycp.pv[3] >> 16) & 0xFFFF);
}

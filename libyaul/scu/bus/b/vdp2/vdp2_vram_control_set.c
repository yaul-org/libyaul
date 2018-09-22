/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/vram.h>

#include "vdp2-internal.h"

void
vdp2_vram_control_set(struct vram_ctl *ctl)
{
        /* VRAM size */
        _internal_state_vdp2.buffered_regs.vrsize = ctl->vram_size;

        /* VRAM mode */
        _internal_state_vdp2.buffered_regs.ramctl &= 0xFCFF;
        _internal_state_vdp2.buffered_regs.ramctl |= ctl->vram_mode;

        _internal_state_vdp2.buffered_regs.ramctl = 0x1300;

        /* Cycle patterns */
        /* The VDP2 is not smart enough to direct all of this VRAM
         * traffic automatically; you must do it yourself */
        _internal_state_vdp2.buffered_regs.cyca0l = (ctl->vram_cycp.pv[0] >> 16) & 0xFFFF;
        _internal_state_vdp2.buffered_regs.cyca0u = ctl->vram_cycp.pv[0] & 0xFFFF;

        _internal_state_vdp2.buffered_regs.cyca1l = (ctl->vram_cycp.pv[1] >> 16) & 0xFFFF;
        _internal_state_vdp2.buffered_regs.cyca1u = ctl->vram_cycp.pv[1] & 0xFFFF;

        _internal_state_vdp2.buffered_regs.cycb0l = (ctl->vram_cycp.pv[2] >> 16) & 0xFFFF;
        _internal_state_vdp2.buffered_regs.cycb0u = ctl->vram_cycp.pv[2] & 0xFFFF;

        _internal_state_vdp2.buffered_regs.cycb1l = (ctl->vram_cycp.pv[3] >> 16) & 0xFFFF;
        _internal_state_vdp2.buffered_regs.cycb1u = ctl->vram_cycp.pv[3] & 0xFFFF;

        MEMORY_WRITE(16, VDP2(VRSIZE), _internal_state_vdp2.buffered_regs.vrsize);
        MEMORY_WRITE(16, VDP2(RAMCTL), _internal_state_vdp2.buffered_regs.ramctl);
        MEMORY_WRITE(16, VDP2(CYCA0L), _internal_state_vdp2.buffered_regs.cyca0l);
        MEMORY_WRITE(16, VDP2(CYCA0U), _internal_state_vdp2.buffered_regs.cyca0u);
        MEMORY_WRITE(16, VDP2(CYCA1L), _internal_state_vdp2.buffered_regs.cyca1l);
        MEMORY_WRITE(16, VDP2(CYCA1U), _internal_state_vdp2.buffered_regs.cyca1u);
        MEMORY_WRITE(16, VDP2(CYCB0L), _internal_state_vdp2.buffered_regs.cycb0l);
        MEMORY_WRITE(16, VDP2(CYCB0U), _internal_state_vdp2.buffered_regs.cycb0u);
        MEMORY_WRITE(16, VDP2(CYCB1L), _internal_state_vdp2.buffered_regs.cycb1l);
        MEMORY_WRITE(16, VDP2(CYCB1U), _internal_state_vdp2.buffered_regs.cycb1u);
}

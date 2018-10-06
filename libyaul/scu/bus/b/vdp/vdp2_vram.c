/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/vram.h>

#include <sys/dma-queue.h>

#include "vdp-internal.h"

void
vdp2_vram_control_set(const struct vram_ctl *vram_ctl)
{
        assert(vram_ctl != NULL);

        assert((vram_ctl->cram_mode == VRAM_CTL_CRAM_MODE_0) ||
               (vram_ctl->cram_mode == VRAM_CTL_CRAM_MODE_1) ||
               (vram_ctl->cram_mode == VRAM_CTL_CRAM_MODE_2));

        assert((vram_ctl->vram_size == VRAM_CTL_SIZE_4MBIT) ||
               (vram_ctl->vram_size == VRAM_CTL_SIZE_8MBIT));

        assert((vram_ctl->vram_mode & 0xFC) == 0x00);

        /* If the coefficient table is set to be stored in CRAM, the
         * color mode must be 1 */
        assert((vram_ctl->coefficient_table == VRAM_CTL_COEFFICIENT_TABLE_VRAM) ||
              ((vram_ctl->coefficient_table == VRAM_CTL_COEFFICIENT_TABLE_CRAM) &&
                vram_ctl->cram_mode == VRAM_CTL_CRAM_MODE_1));

        /* VRAM size */
        _state_vdp2()->regs.vrsize = vram_ctl->vram_size;

        /* Coefficient table storage */
        _state_vdp2()->regs.ramctl &= 0x7FFF;
        _state_vdp2()->regs.ramctl |= vram_ctl->coefficient_table << 15;

        /* CRAM mode */
        _state_vdp2()->regs.ramctl &= 0xCFFF;
        _state_vdp2()->regs.ramctl |= vram_ctl->cram_mode << 12;

        /* VRAM mode */
        _state_vdp2()->regs.ramctl &= 0xFCFF;
        _state_vdp2()->regs.ramctl |= vram_ctl->vram_mode << 8;

        MEMORY_WRITE(16, VDP2(VRSIZE), _state_vdp2()->regs.vrsize);
        MEMORY_WRITE(16, VDP2(RAMCTL), _state_vdp2()->regs.ramctl);
}

void
vdp2_vram_cycp_set(const struct vram_cycp *vram_cycp)
{
        assert(vram_cycp != NULL);

        _state_vdp2()->regs.cyca0l = (vram_cycp->pv[0] >> 16) & 0xFFFF;
        _state_vdp2()->regs.cyca0u = vram_cycp->pv[0] & 0xFFFF;

        _state_vdp2()->regs.cyca1l = (vram_cycp->pv[1] >> 16) & 0xFFFF;
        _state_vdp2()->regs.cyca1u = vram_cycp->pv[1] & 0xFFFF;

        _state_vdp2()->regs.cycb0l = (vram_cycp->pv[2] >> 16) & 0xFFFF;
        _state_vdp2()->regs.cycb0u = vram_cycp->pv[2] & 0xFFFF;

        _state_vdp2()->regs.cycb1l = (vram_cycp->pv[3] >> 16) & 0xFFFF;
        _state_vdp2()->regs.cycb1u = vram_cycp->pv[3] & 0xFFFF;
}

void
vdp2_vram_cycp_clear(void)
{
        _state_vdp2()->regs.cyca0l = 0xFFFF;
        _state_vdp2()->regs.cyca0u = 0xFFFF;

        _state_vdp2()->regs.cyca1l = 0xFFFF;
        _state_vdp2()->regs.cyca1u = 0xFFFF;

        _state_vdp2()->regs.cycb0l = 0xFFFF;
        _state_vdp2()->regs.cycb0u = 0xFFFF;

        _state_vdp2()->regs.cycb1l = 0xFFFF;
        _state_vdp2()->regs.cycb1u = 0xFFFF;
}

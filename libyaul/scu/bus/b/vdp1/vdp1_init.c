/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <common.h>

#include <vdp1.h>
#include <vdp2.h>

#include "vdp1-internal.h"

void
vdp1_init(void)
{
        /* Check if boundaries are correct */
        STATIC_ASSERT((VDP1_CMDT_MEMORY_SIZE +
                VDP1_GST_MEMORY_SIZE +
                VDP1_TEXURE_MEMORY_SIZE +
                VDP1_CLUT_MEMORY_SIZE) == VDP1_VRAM_SIZE);

        /* Initialize the processor to sane values */
        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(ENDR), 0x0000);
        MEMORY_WRITE(16, VDP1(FBCR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);

        MEMORY_WRITE(16, VDP1(EWDR), 0x0000);
        MEMORY_WRITE(16, VDP1(EWLR), 0x0000);
        MEMORY_WRITE(16, VDP1(EWRR), (uint16_t)(((320 / 8) << 9) | (223)));

        vdp2_tvmd_vblank_in_wait();

        /* Stop processing command tables */
        uint32_t cmdt_idx;
        for (cmdt_idx = 0; cmdt_idx < VDP1_CMDT_COUNT_MAX; cmdt_idx++) {
                struct vdp1_cmdt *cmdt;
                cmdt = (struct vdp1_cmdt *)CMD_TABLE(cmdt_idx, 0);
                cmdt->cmd_ctrl = 0x8000;
        }

        vdp1_cmdt_list_init();

        MEMORY_WRITE(16, VDP1(PTMR), 0x0002);
}

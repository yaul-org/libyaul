/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <sys/cdefs.h>

#include <string.h>

#include <vdp1.h>
#include <vdp2.h>

#include "vdp1-internal.h"

static struct vdp1_cmdt _clear_cmd_list[4] = {
        {
                /* System clipping coordinate commands */
                .cmd_ctrl = 0x0009,
                .cmd_link = 0x0000,
                .cmd_pmod = 0x0000,
                .cmd_colr = 0x0000,
                .cmd_srca = 0x0000,
                .cmd_size = 0x0000,
                .cmd_xa = 0x0000,
                .cmd_ya = 0x0000,
                .cmd_xb = 0x0000,
                .cmd_yb = 0x0000,
                .cmd_xc = 0x01FF,
                .cmd_yc = 0x00FF,
                .cmd_xd = 0x0000,
                .cmd_yd = 0x0000,
                .cmd_grda = 0x0000,
                .reserved = 0x0000
        }, {
                /* Local coordinates command */
                .cmd_ctrl = 0x000A,
                .cmd_link = 0x0000,
                .cmd_pmod = 0x0000,
                .cmd_colr = 0x0000,
                .cmd_srca = 0x0000,
                .cmd_size = 0x0000,
                .cmd_xa = 0x0000,
                .cmd_ya = 0x0000,
                .cmd_xb = 0x0000,
                .cmd_yb = 0x0000,
                .cmd_xc = 0x0000,
                .cmd_yc = 0x0000,
                .cmd_xd = 0x0000,
                .cmd_yd = 0x0000,
                .cmd_grda = 0x0000,
                .reserved = 0x0000
        }, {
                /* Polygon */
                .cmd_ctrl = 0x0004,
                .cmd_link = 0x0000,
                .cmd_pmod = 0x00C0,
                .cmd_colr = 0x8000,
                .cmd_srca = 0x0000,
                .cmd_size = 0x0000,
                .cmd_xa = 0x0000,
                .cmd_ya = 0x00FF,
                .cmd_xb = 0x01FF,
                .cmd_yb = 0x00FF,
                .cmd_xc = 0x01FF,
                .cmd_yc = 0x0000,
                .cmd_xd = 0x0000,
                .cmd_yd = 0x0000,
                .cmd_grda = 0x0000,
                .reserved = 0x0000
        }, {
                .cmd_ctrl = 0x8000,
                .cmd_link = 0x0000,
                .cmd_pmod = 0x0000,
                .cmd_colr = 0x0000,
                .cmd_srca = 0x0000,
                .cmd_size = 0x0000,
                .cmd_xa = 0x0000,
                .cmd_ya = 0x0000,
                .cmd_xb = 0x0000,
                .cmd_yb = 0x0000,
                .cmd_xc = 0x0000,
                .cmd_yc = 0x0000,
                .cmd_xd = 0x0000,
                .cmd_yd = 0x0000,
                .cmd_grda = 0x0000,
                .reserved = 0x0000
        }
};

void
vdp1_init(void)
{
        /* Check if boundaries are correct */
        static_assert((VDP1_CMDT_MEMORY_SIZE +
                       VDP1_GST_MEMORY_SIZE +
                       VDP1_TEXURE_MEMORY_SIZE +
                       VDP1_CLUT_MEMORY_SIZE) == VDP1_VRAM_SIZE);

        /* Initialize the processor to sane values */
        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(FBCR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
        /* Force stop immediately */
        MEMORY_WRITE(16, VDP1(ENDR), 0x0000);

        MEMORY_WRITE(16, VDP1(EWDR), 0x0000);
        MEMORY_WRITE(16, VDP1(EWLR), 0x0000);
        MEMORY_WRITE(16, VDP1(EWRR), (uint16_t)(((512 / 8) << 9) | (255)));

        /* Need to write half words */
        // memcpy((void *)CMD_TABLE(0, 0), _clear_cmd_list, sizeof(_clear_cmd_list));

        /* /\* Start drawing immediately *\/ */
        /* MEMORY_WRITE(16, VDP1(PTMR), 0x0001); */
        /* /\* Wait until VDP1 finishes drawing (CEF = BEF = 1) *\/ */
        /* while ((MEMORY_READ(16, VDP1(EDSR)) & 0x0003) == 0x0000); */
        /* MEMORY_WRITE(16, VDP1(PTMR), 0x0000); */

        /* Switch to 1-cycle mode */
        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(FBCR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
}

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

static struct vdp1_cmdt _init_cmd_list[4] = {
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
        /* Idle */
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
        /* Force stop immediately */
        MEMORY_WRITE(16, VDP1(ENDR), 0x0000);

        /* Wait 30 cycles? */

        MEMORY_WRITE(16, VDP1(EWDR), 0x0000);
        MEMORY_WRITE(16, VDP1(EWLR), 0x0000);
        MEMORY_WRITE(16, VDP1(EWRR), (uint16_t)(((512 / 8) << 9) | (255)));

        /* Need to write half words */
        memcpy((void *)CMD_TABLE(0, 0), _init_cmd_list, sizeof(_init_cmd_list));

        /*-
         * 1. Erase FB #1
         * 2. Change frame buffer
         * 3. Erase FB #0
         * 4. Change frame buffer
         */
        uint32_t loop;
        for (loop = 0; loop < 2; loop++) {
                /* Start drawing immediately */
                MEMORY_WRITE(16, VDP1(PTMR), 0x0001);
                /* Wait until VDP1 finishes drawing (CEF = 1) */
                while ((MEMORY_READ(16, VDP1(EDSR)) & 0x0002) != 0x0002) {
                }
                /* Idle */
                MEMORY_WRITE(16, VDP1(PTMR), 0x0000);

                /* Change FB */
                MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
                /* FCM = FCT = 1 */
                MEMORY_WRITE(16, VDP1(FBCR), 0x0003);
                /* Wait for change of FB (CEF = 1) */
                while ((MEMORY_READ(16, VDP1(EDSR)) & 0x0002) == 0x0002) {
                }
        }

        /* Clear VDP1 command table area */
        uint32_t cmdt_idx;
        for (cmdt_idx = 0; cmdt_idx < VDP1_CMDT_COUNT_MAX; cmdt_idx++) {
                struct vdp1_cmdt *cmdt;
                cmdt = (struct vdp1_cmdt *)CMD_TABLE(cmdt_idx, 0);

                cmdt->cmd_ctrl = 0x8000;
        }

        vdp1_cmdt_list_init();

        /* Switch to 1-cycle mode */
        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(FBCR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
}

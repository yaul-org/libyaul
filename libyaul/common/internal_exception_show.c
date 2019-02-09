/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include <cpu/intc.h>

#include <scu/ic.h>

#include <vdp1.h>
#include <vdp2.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include <vdp.h>

#include <sys/dma-queue.h>

#include <dbgio.h>

void __noreturn
_internal_exception_show(const char *buffer)
{
        cpu_intc_mask_set(15);
        scu_ic_mask_chg(IC_MASK_NONE, IC_MASK_ALL);

        vdp2_tvmd_display_res_set(TVMD_INTERLACE_NONE, TVMD_HORZ_NORMAL_A,
            TVMD_VERT_224);
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(0, 0x01FFFE),
            COLOR_RGB555(0, 7, 0));

        vdp2_scrn_display_clear();

        vdp2_tvmd_display_set();

        vdp1_env_default_set();

        vdp2_sprite_priority_set(0, 0);
        vdp2_sprite_priority_set(1, 0);
        vdp2_sprite_priority_set(2, 0);
        vdp2_sprite_priority_set(3, 0);
        vdp2_sprite_priority_set(4, 0);
        vdp2_sprite_priority_set(5, 0);
        vdp2_sprite_priority_set(6, 0);
        vdp2_sprite_priority_set(7, 0);

        /* Clear any pending VDP sync user callbacks */
        vdp_sync_user_callback_clear();

        vdp_sync_vblank_in_clear();
        vdp_sync_vblank_out_clear();

        dma_queue_clear();

        dbgio_dev_default_init(DBGIO_DEV_VDP2);
        dbgio_dev_set(DBGIO_DEV_VDP2);
        dbgio_buffer(buffer);

        dbgio_flush();
        vdp_sync(0);

        while (true) {
        }
}

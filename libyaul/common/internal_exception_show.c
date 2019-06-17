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

static void _vblank_in_handler(void);

void __noreturn
_internal_exception_show(const char *buffer)
{
        cpu_intc_mask_set(15);
        scu_ic_mask_chg(IC_MASK_NONE, IC_MASK_ALL);

        vdp2_tvmd_display_res_set(VDP2_TVMD_INTERLACE_NONE, VDP2_TVMD_HORZ_NORMAL_A,
            VDP2_TVMD_VERT_224);
        vdp2_scrn_back_screen_color_set(VDP2_VRAM_ADDR(0, 0x01FFFE),
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

        dbgio_dev_default_init(DBGIO_DEV_VDP2_SIMPLE);
        dbgio_buffer(buffer);

        scu_ic_ihr_set(IC_INTERRUPT_VBLANK_IN, _vblank_in_handler);

        scu_ic_mask_chg(~IC_MASK_VBLANK_IN, IC_MASK_NONE);
        cpu_intc_mask_set(14);

        while (true) {
        }

        __builtin_unreachable();
}

static void
_vblank_in_handler(void)
{
        dbgio_flush();

        /* Synchronize VDP2 only.
         *
         * Avoid using vdp_sync() as we don't need to sync VDP2 and can no
         * longer fire off any interrupts */
        vdp2_sync_commit();
}

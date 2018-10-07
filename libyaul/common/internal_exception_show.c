/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include <cpu/intc.h>

#include <vdp1.h>
#include <vdp2.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include <sys/dma-queue.h>

#include <cons.h>

void __noreturn
_internal_exception_show(const char *buffer)
{
        cpu_intc_mask_set(15);

        /* Reset the VDP2 */
        vdp2_tvmd_display_res_set(TVMD_INTERLACE_NONE, TVMD_HORZ_NORMAL_A,
            TVMD_VERT_224);
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(0, 0x01FFFE),
            COLOR_RGB555(0, 7, 0));

        vdp2_tvmd_display_clear();

        vdp2_scrn_display_clear();

        vdp2_sprite_type_set(0);
        vdp2_sprite_priority_set(0, 0);

        dma_queue_clear();

        cons_init(CONS_DRIVER_VDP2, 40, 28);
        cons_buffer(buffer);

        vdp2_commit_handler_set(NULL, NULL);
        vdp2_commit();

        vdp2_tvmd_vblank_out_wait();
        vdp2_tvmd_vblank_in_wait();
        dma_queue_flush(DMA_QUEUE_TAG_VBLANK_IN);
        cons_flush();

        vdp2_commit_wait();

        vdp2_tvmd_display_set();

        abort();
}

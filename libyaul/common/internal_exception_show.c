/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include <vdp1.h>
#include <vdp2.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include <cons.h>

void __noreturn
internal_exception_show(const char *buffer)
{
        /* Reset the VDP1 */
        vdp1_init();

        /* Reset the VDP2 */
        vdp2_init();
        vdp2_tvmd_display_res_set(TVMD_INTERLACE_NONE, TVMD_HORZ_NORMAL_A,
            TVMD_VERT_224);
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(0, 0x01FFFE),
            COLOR_RGB555(0, 7, 0));

        /* Set sprite to type 0 and set its priority to 0 (invisible) */
        vdp2_sprite_type_set(0);
        vdp2_sprite_priority_set(0, 0);

        vdp2_tvmd_display_set();

        cons_init(CONS_DRIVER_VDP2, 40, 28);
        cons_buffer(buffer);

        vdp2_tvmd_vblank_out_wait();
        vdp2_tvmd_vblank_in_wait();
        vdp2_commit();
        cons_flush();

        abort();
}

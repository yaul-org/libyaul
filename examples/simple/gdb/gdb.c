/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include <assert.h>

void jsr(void);

int
main(void)
{
        uint16_t blcs_color[] = {
                0x9C00
        };

        vdp2_init();
        vdp2_scrn_back_screen_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x01FFFE),
            blcs_color, 0);

        gdb_init();

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }

        return 0;
}

/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacquez <mrko@gmail.com>
 */

#include <vdp2.h>
#include <gdb.h>

#include <assert.h>

void jsr(void);

int
main(void)
{
        uint16_t blcs_color[] = {
                0x9C00
        };

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x01FFFE),
            blcs_color, 0);

        gdb_init();

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }

        return 0;
}

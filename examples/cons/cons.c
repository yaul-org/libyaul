/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2.h>

#include <cons/vdp2.h>

int
main(void)
{
        uint16_t blcs_color[] = {
                0x9C00
        };

        struct cons cons;

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        cons_vdp2_init(&cons);
        cons_write(&cons, "Hello, World!");

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }

        return 0;
}

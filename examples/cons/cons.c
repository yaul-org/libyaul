/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>

#include <cons/vdp2.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void hardware_init(void);

int
main(void)
{

        hardware_init();

        struct cons cons;

        cons_vdp2_init(&cons);

        char *buffer;

        buffer = (char *)malloc(8192);
        assert(buffer != NULL);

        memset(buffer, 0x00, 8192);

        (void)sprintf(buffer,
            "[1;32;1;45m   *** Console (\"cons\" component) ***   [m\n"
            "\n"
            "[H");

        while (true) {
                vdp2_tvmd_vblank_out_wait();
                vdp2_tvmd_vblank_in_wait();
                /* VBLANK */
                cons_write(&cons, buffer);
        }

        return 0;
}

static void
hardware_init(void)
{
        uint16_t blcs_color[] = {
                0x9C00
        };

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();
        smpc_peripheral_init();
}

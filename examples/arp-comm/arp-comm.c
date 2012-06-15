/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>
#include <arp.h>
#include <cpu/intc.h>

#include <cons/vdp2.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static struct cons cons;

static void
local_arp_cb(arp_callback_t *arp_cb)
{
        char *buf;

        buf = (char *)malloc(1024);
        assert(buf != NULL);

        (void)sprintf(buf, "Callback\n"
            "ptr: %p\n"
            "len: 0x%04X\n"
            "function: 0x%02X\n\n",
            arp_cb->ptr,
            arp_cb->len,
            arp_cb->function);

        cons_write(&cons, buf);

        free(buf);
}

int
main(void)
{
        uint16_t blcs_color[] = {
                0x9C00
        };

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_vdp2_init(&cons);
        cons_write(&cons, "\n[1;44m     *** ARP Communication test ***     [m\n\n");

        /* Register callback */
        arp_function_callback(&local_arp_cb);

        cons_write(&cons, "Ready...\n\n");

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                arp_function_nonblock();
        }

        return 0;
}

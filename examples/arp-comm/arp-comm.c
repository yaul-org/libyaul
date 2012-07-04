/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
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
            "function: 0x%02X\n"
            "execute: %s[5;1H;",
            arp_cb->ptr,
            arp_cb->len,
            arp_cb->function,
            (arp_cb->exec ? "yes" : "no"));

        cons_write(&cons, buf);

        free(buf);
}

int
main(void)
{
        char *text;
        char *arp_ver;

        uint16_t blcs_color[] = {
                0x9C00
        };

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_vdp2_init(&cons);

        if ((text = (char *)malloc(1024)) == NULL)
                abort();

        cons_write(&cons, "\n[1;44m     *** ARP Communication test ***     [m\n\n");

        cons_write(&cons, "Initializing ARP...\n");
        arp_ver = arp_version();

        if (*arp_ver == '\0') {
                cons_write(&cons, "No ARP cartridge detected!\n");
                abort();
        }

        (void)sprintf(text, "ARP version \"%s\" detected!\n", arp_ver);
        cons_write(&cons, text);
        free(arp_ver);

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

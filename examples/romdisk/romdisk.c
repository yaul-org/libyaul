/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>
#include <dram-cartridge.h>
#include <arp.h>

#include <cons/vdp2.h>

#include <romdisk/romdisk.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void delay(uint16_t);

/* Address of ROMDISK root */
static void *root_romdisk = NULL;

static void
local_arp_cb(arp_callback_t *arp_cb)
{

        if (arp_cb->function == 0x09)
                root_romdisk = arp_cb->ptr;
}

int
main(void)
{
        static char buf[1024];

        uint16_t blcs_color[] = {
                0x9C00
        };

        struct cons cons;

        void *romdisk;

        char *msg; /* Buffer for file */
        ssize_t msg_len;
        void *fh; /* File handle */

        uint32_t *cart;
        size_t cart_len;

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_vdp2_init(&cons);
        cons_write(&cons, "\n[1;44m          *** ROMDISK Test ***          [m\n\n");

        cons_write(&cons, "Initializing DRAM cartridge...\n");
        dram_cartridge_init();

        delay(2);

        cart = (uint32_t *)dram_cartridge_area();
        if (cart == NULL) {
                cons_write(&cons, "[4;1H[2K[11CThe extended RAM\n"
                    "[11Ccartridge is not\n"
                    "[11Cinsert properly.\n"
                    "\n"
                    "[11CPlease turn off\n"
                    "[11Cpower and reinsert\n"
                    "[11Cthe extended RAM\n"
                    "[11Ccartridge.\n");

                abort();
        }

        cart_len = dram_cartridge_size();
        (void)sprintf(buf, "%s DRAM Cartridge detected\n",
            ((cart_len == 0x00080000)
                ? "8-Mbit"
                : "32-Mbit"));
        cons_write(&cons, buf);

        cons_write(&cons, "Initializing ARP... ");
        arp_function_callback(&local_arp_cb);
        cons_write(&cons, "OK!\n");

        cons_write(&cons, "Mounting ROMDISK... ");
        romdisk_init();

        /* Wait until it's fully transferred */
        while (root_romdisk == NULL)
                arp_function_nonblock();
        romdisk = romdisk_mount("/", root_romdisk);
        delay(1);
        if (romdisk != NULL)
                cons_write(&cons, "OK!\n");

        cons_write(&cons, "Opening \"/hello.world\" ");
        if ((fh = romdisk_open(romdisk, "/hello.world", O_RDONLY)) == NULL) {
                cons_write(&cons, "FAILED\n");
                abort();
        }
        cons_write(&cons, "OK!\n");

        msg_len = romdisk_total(fh);
        msg = (char *)malloc(msg_len + 1);
        assert(msg != NULL);
        memset(msg, '\0', msg_len + 1);

        cons_write(&cons, "Reading... ");
        msg_len = romdisk_read(fh, msg, romdisk_total(fh));
        if (msg_len == 0) {
                cons_write(&cons, "FAILED\n");
                abort();
        }

        cons_write(&cons, "\n[1;33m");
        cons_write(&cons, msg);
        cons_write(&cons, "[m");

        cons_write(&cons, "\nTest complete!\n");

        abort();
}

static void
delay(uint16_t t)
{
        uint16_t frame;

        for (frame = 0; frame < (60 * t); frame++) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }
}

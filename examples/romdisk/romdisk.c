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
#include <fs/romdisk/romdisk.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern uint8_t root_romdisk[];

static void delay(uint16_t);

int
main(void)
{
        uint16_t blcs_color[] = {
                0x9C00
        };

        struct cons cons;

        void *romdisk;

        char *msg; /* Buffer for file */
        ssize_t msg_len;
        void *fh; /* File handle */

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_vdp2_init(&cons);
        cons_write(&cons, "\n[1;44m          *** ROMDISK Test ***          [m\n\n");

        cons_write(&cons, "Mounting ROMDISK... ");
        romdisk_init();

        romdisk = romdisk_mount("/", root_romdisk);

        delay(1);

        if (romdisk != NULL)
                cons_write(&cons, "OK!\n");

        delay(1);

        cons_write(&cons, "Opening \"/tmp/txt/hello.world\" ");
        if ((fh = romdisk_open(romdisk, "/tmp/txt/hello.world", O_RDONLY)) == NULL) {
                cons_write(&cons, "[1;31mFAILED[0m\n");
                abort();
        }

        delay(1);

        cons_write(&cons, "OK!\n");

        msg_len = romdisk_total(fh);
        msg = (char *)malloc(msg_len + 1);
        assert(msg != NULL);
        memset(msg, '\0', msg_len + 1);

        delay(1);

        cons_write(&cons, "Reading... ");
        msg_len = romdisk_read(fh, msg, romdisk_total(fh));

        delay(1);

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

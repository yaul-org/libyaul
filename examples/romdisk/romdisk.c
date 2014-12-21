/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>

#include <cons.h>
#include <fs/romdisk/romdisk.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern uint8_t root_romdisk[];

static void delay(uint16_t);
static void sync(void);

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
        vdp2_scrn_back_screen_set(/* single_color = */ true, VRAM_ADDR_4MBIT(2, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_init(&cons, CONS_DRIVER_VDP2);

        cons_buffer(&cons, "\n[1;44m          *** ROMDISK Test ***          [m\n\n");

        cons_buffer(&cons, "Mounting ROMDISK... ");

        romdisk_init();

        romdisk = romdisk_mount("/", root_romdisk);

        sync();
        cons_write(&cons);

        delay(1);

        if (romdisk != NULL) {
                cons_buffer(&cons, "OK!\n");
                sync();
                cons_write(&cons);
        }

        delay(1);

        cons_buffer(&cons, "Opening \"/tmp/txt/hello.world\"... ");
        sync();
        cons_write(&cons);
        if ((fh = romdisk_open(romdisk, "/tmp/txt/hello.world", O_RDONLY)) == NULL) {
                cons_buffer(&cons, "[1;31mFAILED[0m\n");
                sync();
                cons_write(&cons);
                abort();
        }

        delay(1);

        cons_buffer(&cons, "OK!\n");
        sync();
        cons_write(&cons);

        msg_len = romdisk_total(fh);
        msg = (char *)malloc(msg_len + 1);
        assert(msg != NULL);
        memset(msg, '\0', msg_len + 1);

        delay(1);

        cons_buffer(&cons, "Reading... ");
        sync();
        cons_write(&cons);
        msg_len = romdisk_read(fh, msg, romdisk_total(fh));

        delay(1);

        cons_buffer(&cons, "OK!\n");
        sync();
        cons_write(&cons);

        if (msg_len == 0) {
                cons_buffer(&cons, "FAILED\n");
                sync();
                cons_write(&cons);
                abort();
        }

        cons_buffer(&cons, "\n[1;33m");
        cons_buffer(&cons, msg);
        cons_buffer(&cons, "[m");

        cons_buffer(&cons, "\nTest complete!\n");
        sync();
        cons_write(&cons);

        abort();
}

static void
delay(uint16_t t)
{
        uint16_t frame;

        for (frame = 0; frame < (60 * t); frame++) {
                sync();
        }
}

static void
sync(void)
{
        vdp2_tvmd_vblank_out_wait();
        vdp2_tvmd_vblank_in_wait();
}

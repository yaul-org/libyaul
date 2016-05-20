/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

extern uint8_t root_romdisk[];

#define SYS_CHGSYSCK(_CkMode_) ((**(volatile void(**)(uint32_t))0x6000320)(_CkMode_))
#define SYS_GETSYSCK (*(volatile uint32_t*)0x6000324)

static void delay(uint16_t);

int
main(void)
{
        void *romdisk;

        char *msg; /* Buffer for file */
        ssize_t msg_len;
        void *fh; /* File handle */

        vdp2_init();
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(2, 0x01FFFE),
            COLOR_RGB555(0, 0, 7));

        smpc_init();

        cons_init(CONS_DRIVER_VDP2, 40, 28);

        cons_write("\n[1;44m          *** ROMDISK Test ***          [m\n\n");

        cons_write("Mounting ROMDISK... ");

        romdisk_init();

        romdisk = romdisk_mount("/", root_romdisk);

        delay(1);

        if (romdisk != NULL) {
                cons_write("OK!\n");
        }

        delay(1);

        cons_write("Opening \"/tmp/txt/hello.world\"... ");
        if ((fh = romdisk_open(romdisk, "/tmp/txt/hello.world", O_RDONLY)) == NULL) {
                cons_write("[1;31mFAILED[0m\n");
                abort();
        }

        delay(1);

        cons_write("OK!\n");

        msg_len = romdisk_total(fh);
        msg = (char *)malloc(msg_len + 1);
        assert(msg != NULL);
        memset(msg, '\0', msg_len + 1);

        delay(1);

        cons_write("Reading... ");
        msg_len = romdisk_read(fh, msg, romdisk_total(fh));

        delay(1);

        cons_write("OK!\n");

        if (msg_len == 0) {
                cons_write("FAILED\n");
                abort();
        }

        cons_write("\n[1;32m");
        cons_write(msg);
        cons_write("[m");

        cons_write("\nTest complete!\n");

        while (true) {
                vdp2_tvmd_vblank_out_wait();
                vdp2_tvmd_vblank_in_wait();
        }
}

static void
delay(uint16_t t)
{
        uint16_t frame;

        for (frame = 0; frame < (60 * t); frame++) {
                vdp2_tvmd_vblank_out_wait();
                vdp2_tvmd_vblank_in_wait();
        }
}

/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

static void delay(uint16_t);
static bool xchg(uint32_t, uint32_t);

static uint32_t *cart_area = NULL;

int
main(void)
{
        bool passed;
        char *result;
        uint32_t x;
        char *buf;

        size_t cart_len;
        uint32_t id;

        vdp2_init();
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(3, 0x01FFFE),
            COLOR_RGB555(0, 0, 7));

        smpc_init();

        cons_init(CONS_DRIVER_VDP2, 40, 28);

        cons_write("\n[1;44m      *** DRAM Cartridge Test ***       [m\n\n");

        cons_write("Initializing DRAM cartridge... ");
        dram_cartridge_init();

        cons_write("OK!\n");

        delay(2);

        id = dram_cartridge_id();
        if ((id != DRAM_CARTRIDGE_ID_1MIB) && (id != DRAM_CARTRIDGE_ID_4MIB)) {
                cons_write("[4;1H[2K[11CThe extended RAM\n"
                    "[11Ccartridge is not\n"
                    "[11Cinsert properly.\n"
                    "\n"
                    "[11CPlease turn off\n"
                    "[11Cpower and reinsert\n"
                    "[11Cthe extended RAM\n"
                    "[11Ccartridge.\n");
                abort();
        }

        buf = (char *)malloc(1024);
        assert(buf != NULL);

        cart_area = (uint32_t *)dram_cartridge_area();
        cart_len = dram_cartridge_size();
        (void)sprintf(buf, "%s DRAM Cartridge detected\n",
            ((id == DRAM_CARTRIDGE_ID_1MIB)
                ? "8-Mbit"
                : "32-Mbit"));
        cons_write(buf);

        for (x = 0; x < cart_len / sizeof(x); x++) {
                vdp2_tvmd_vblank_out_wait();

                passed = xchg(x, x);
                result = passed ? "OK!" : "FAILED!";
                (void)sprintf(buf, "[7;1HWriting to 0x%08X (%d%%) %s\n",
                    (uintptr_t)&cart_area[x], (int)(x / cart_len), result);
                cons_buffer(buf);

                vdp2_tvmd_vblank_in_wait();
                cons_flush();

                if (!passed) {
                        break;
                }
        }

        cons_write(passed ? "Test is complete!" : "Test was aborted!");
        free(buf);

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

static bool
xchg(uint32_t v, uint32_t ofs)
{
        uint32_t w;

        cart_area[ofs] = v;
        w = cart_area[ofs];

        return (w == v);
}

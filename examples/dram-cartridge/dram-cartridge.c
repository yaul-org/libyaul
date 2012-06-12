/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2.h>
#include <smpc.h>
#include <smpc/peripheral.h>
#include <dram-cartridge.h>

#include <cons/vdp2.h>

#include <stdio.h>
#include <stdbool.h>

static void delay(uint16_t);
static bool xchg(uint32_t, uint32_t);

int
main(void)
{
        static char buf[1024];

        uint16_t blcs_color[] = {
                0x9C00
        };

        struct cons cons;

        char *result;
        uint32_t x;

        uint32_t *cart;
        size_t cart_len;

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        smpc_init();

        cons_vdp2_init(&cons);
        cons_write(&cons, "\n[1;44m      *** DRAM Cartridge Test ***       [m\n\n");

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

                while (true) {
                        vdp2_tvmd_vblank_in_wait();
                        vdp2_tvmd_vblank_out_wait();
                }
        }

        cart_len = dram_cartridge_size();
        (void)sprintf(buf, "%s DRAM Cartridge detected\n",
            ((cart_len == 0x00080000)
                ? "16-Mbit"
                : "32-Mbit"));
        cons_write(&cons, buf);

        for (x = 0; x < cart_len / sizeof(x); x++) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                result = (xchg(x, x)) ? "OK" : "Failed!";

                (void)sprintf(buf, "[7;1HWriting 0x%08lX to 0x%08X %s\n",
                    x, (uintptr_t)&cart[x], result);

                cons_write(&cons, buf);
        }

        cons_write(&cons, "\nTest is complete!");

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }

        return 0;
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

static bool
xchg(uint32_t v, uint32_t ofs)
{
        uint32_t w;
        uint32_t *cart;

        if ((cart = (uint32_t *)dram_cartridge_area()) == NULL)
                return false;

        cart[ofs] = v;
        w = cart[ofs];

        return (w == v);
}

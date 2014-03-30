/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/dmac.h>
#include <cpu/intc.h>
#include <vdp2.h>

#include <cons/vdp2.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static struct cons cons;

static void
dma_complete(void)
{
        cons_write(&cons, "[6;1HDMA transfer on channel A is complete");
        abort();
}

int
main(void)
{
        uint16_t blcs_color[] = {
                0x9C00
        };

        char *text;
        uint16_t frame;

        struct cpu_channel_cfg cfg = {
                .ch = CPU_DMAC_CHANNEL(0),
                .dst = {
                        .mode = CPU_DMAC_DESTINATION_INCREMENT,
                        .ptr = (void *)0x20000000
                },

                .src = {
                        .mode = CPU_DMAC_SOURCE_INCREMENT,
                        .ptr = (void *)0x20000000
                },

                .len = 0x00100000,
                .xfer_size = 1,
                .priority = 15,
                .vector = 127,
                .ihr = dma_complete
        };

        if ((text = malloc(1024)) == NULL)
                abort();

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x01FFFE),
            blcs_color, 0);

        cons_vdp2_init(&cons);

        cons_write(&cons, "\n[1;44m         *** CPU DMAC test ***          [m\n\n");
        cons_write(&cons, "Transferring 1MiB in 1-byte strides\n");

        cfg = cfg;
        cpu_dmac_channel_stop();
        cpu_dmac_channel_set(&cfg);
        cpu_dmac_channel_start(CPU_DMAC_CHANNEL(0));

        frame = 0;
        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
                (void)sprintf(text, "[5;1HNumber of frames %i\n", frame);
                cons_write(&cons, text);
                frame++;
        }

        return 0;
}

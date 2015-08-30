/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <yaul.h>

#include "globals.h"
#include "fs.h"
#include "scene.h"

/* Scenes */
#include "game-over.h"
#include "game.h"
#include "title.h"
#include "tutorial.h"

static uint32_t tick = 0;

static void vblank_in_handler(irq_mux_handle_t *);
static void vblank_out_handler(irq_mux_handle_t *);

static void
hardware_init(void)
{
        /* VDP2 */
        vdp2_init();

        /* VDP1 */
        vdp1_init();

        /* SMPC */
        smpc_init();
        smpc_peripheral_init();

        /* Disable interrupts */
        cpu_intc_disable();

        irq_mux_t *vblank_in;
        irq_mux_t *vblank_out;

        vblank_in = vdp2_tvmd_vblank_in_irq_get();
        irq_mux_handle_add(vblank_in, vblank_in_handler, NULL);

        vblank_out = vdp2_tvmd_vblank_out_irq_get();
        irq_mux_handle_add(vblank_out, vblank_out_handler, NULL);

        /* Enable interrupts */
        cpu_intc_enable();
}

int
main(void)
{
        hardware_init();

        fs_init();
        scene_init();

        scene_register("title", title_init, title_update, title_draw, title_exit);
        scene_register("tutorial", tutorial_init, tutorial_update, tutorial_draw, tutorial_exit);
        scene_register("game", game_init, game_update, game_draw, game_exit);
        scene_register("game-over", game_over_init, game_over_update, game_over_draw, game_over_exit);

        scene_load("title");

        while (true) {
                vdp2_tvmd_vblank_out_wait();
                scene_update();

                vdp2_tvmd_vblank_in_wait();
                /* VBLANK */
                scene_draw();
        }

        return 0;
}

static void
vblank_in_handler(irq_mux_handle_t *irq_mux __unused)
{
        g_frame_counter = (tick > 0) ? (g_frame_counter + 1) : 0;
        smpc_peripheral_digital_port(1, &g_digital);
}

static void
vblank_out_handler(irq_mux_handle_t *irq_mux __unused)
{
        tick = (tick & 0xFFFFFFFF) + 1;
}

/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "fs.h"
#include "globals.h"
#include "state.h"

extern uint8_t root_romdisk[];

static void vblank_in_handler(irq_mux_handle_t *);
static void vblank_out_handler(irq_mux_handle_t *);

static void hardware_init(void);

static struct state_data state_data;

int
main(void)
{
        hardware_init();
        fs_init();
        cons_init(&cons, CONS_DRIVER_VDP2);

        state_machine_init(&state_game);
        state_machine_add_state(&state_game, "game",
            STATE_GAME_GAME,
            state_game_init,
            state_game_update,
            state_game_draw,
            state_game_exit,
            &state_data);

        state_machine_transition(&state_game, STATE_GAME_GAME);

        while (true) {
                vdp2_tvmd_vblank_out_wait();
                state_machine_handler_update(&state_game);
                vdp2_tvmd_vblank_in_wait();
                state_machine_handler_draw(&state_game);
        }

        return 0;
}

static void
hardware_init(void)
{
        static uint16_t blcs_color[] = {
                0x9C00
        };

        /* VDP1 */
        vdp1_init();

        /* VDP2 */
        vdp2_init();

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

        vdp2_scrn_back_screen_set(/* single_color = */ true,
            VRAM_ADDR_4MBIT(2, 0x1FFFE), blcs_color, 1);
}

static void
vblank_in_handler(irq_mux_handle_t *irq_mux __unused)
{
        smpc_peripheral_digital_port(1, &digital_pad);
}

static void
vblank_out_handler(irq_mux_handle_t *irq_mux __unused)
{
        if ((vdp2_tvmd_vcount_get()) == 0) {
                tick = (tick & 0xFFFFFFFF) + 1;
        }
}

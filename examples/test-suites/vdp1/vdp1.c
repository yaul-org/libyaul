/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

#include "fs.h"
#include "globals.h"
#include "state.h"
#include "tests.h"

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   224

extern uint8_t root_romdisk[];

static void vblank_in_handler(irq_mux_handle_t *);
static void vblank_out_handler(irq_mux_handle_t *);

static void hardware_init(void);

static struct state_data {
        struct {
                int32_t type;
                int32_t subtype;
                int32_t idx;
        } test;
} state_data;

static void state_00_init(struct state_context *);
static void state_00_update(struct state_context *);
static void state_00_draw(struct state_context *);
static void state_00_exit(struct state_context *);

static void state_01_init(struct state_context *);
static void state_01_update(struct state_context *);
static void state_01_draw(struct state_context *);
static void state_01_exit(struct state_context *);

int
main(void)
{
        hardware_init();

        fs_init();
        cons_init(CONS_DRIVER_VDP2);

        state_machine_init(&state_vdp1);
        state_machine_add_state(&state_vdp1, "menu",
            STATE_VDP1_MENU,
            state_00_init,
            state_00_update,
            state_00_draw,
            state_00_exit,
            &state_data);
        state_machine_add_state(&state_vdp1, "testing",
            STATE_VDP1_TESTING,
            state_01_init,
            state_01_update,
            state_01_draw,
            state_01_exit,
            &state_data);

        state_data.test.type = TEST_TYPE_PRIMITIVES;
        state_data.test.subtype = TEST_SUBTYPE_SCALED_SPRITE;
        state_data.test.idx = 0;

        state_machine_transition(&state_vdp1, STATE_VDP1_TESTING);

        vdp2_tvmd_vblank_out_wait();
        vdp2_tvmd_vblank_in_wait();

        while (true) {
                vdp2_tvmd_vblank_out_wait();
                state_machine_handler_update(&state_vdp1);
                vdp2_tvmd_vblank_in_wait();
                state_machine_handler_draw(&state_vdp1);
        }

        return 0;
}

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

        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(2, 0x01FFFE), 0x9C00);

        /* Turn on display */
        vdp2_tvmd_display_set();
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

static void
state_00_init(struct state_context *state_context __unused)
{
}

static void
state_00_update(struct state_context *state_context __unused)
{
}

static void
state_00_draw(struct state_context *state_context __unused)
{
}

static void
state_00_exit(struct state_context *state_context __unused)
{
}

static void
state_01_init(struct state_context *state_context)
{
        struct state_data *state_data;
        state_data = (struct state_data *)state_context->sc_data;

        const struct test *current_test;
        current_test = tests_fetch(state_data->test.type,
            state_data->test.subtype,
            state_data->test.idx);

        current_test->init();
}

static void
state_01_update(struct state_context *state_context)
{
        struct state_data *state_data;
        state_data = (struct state_data *)state_context->sc_data;

        const struct test *current_test;
        current_test = tests_fetch(state_data->test.type,
            state_data->test.subtype,
            state_data->test.idx);

        current_test->update();
}

static void
state_01_draw(struct state_context *state_context)
{
        struct state_data *state_data;
        state_data = (struct state_data *)state_context->sc_data;

        const struct test *current_test;
        current_test = tests_fetch(state_data->test.type,
            state_data->test.subtype,
            state_data->test.idx);

        current_test->draw();
}

static void
state_01_exit(struct state_context *state_context)
{
        struct state_data *state_data __unused;
        state_data = (struct state_data *)state_context->sc_data;

        const struct test *current_test;
        current_test = tests_fetch(state_data->test.type,
            state_data->test.subtype,
            state_data->test.idx);

        current_test->exit();
}

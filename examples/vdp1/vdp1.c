/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"

#define TEST_DECLARE_PROTOTYPE(n)                                              \
    void CC_CONCAT(CC_CONCAT(test_, n),_init(void));                           \
    void CC_CONCAT(CC_CONCAT(test_, n),_update(void));                         \
    void CC_CONCAT(CC_CONCAT(test_, n),_draw(void));                           \
    void CC_CONCAT(CC_CONCAT(test_, n),_exit(void))

extern uint8_t root_romdisk[];

/* Globals */
struct cons cons;
struct smpc_peripheral_digital digital_pad;
uint32_t tick = 0;

TEST_DECLARE_PROTOTYPE(00);
TEST_DECLARE_PROTOTYPE(01);
TEST_DECLARE_PROTOTYPE(02);
TEST_DECLARE_PROTOTYPE(03);
TEST_DECLARE_PROTOTYPE(04);
TEST_DECLARE_PROTOTYPE(05);
TEST_DECLARE_PROTOTYPE(06);
TEST_DECLARE_PROTOTYPE(07);
TEST_DECLARE_PROTOTYPE(08);

static void vblank_in_handler(irq_mux_handle_t *);
static void vblank_out_handler(irq_mux_handle_t *);

static void hardware_init(void);

static struct test {
        char *test_name;
        void (*test_init)(void);
        void (*test_update)(void);
        void (*test_draw)(void);
        void (*test_exit)(void);
} tests[] = {
        {
                "polygon",
                test_00_init,
                test_00_update,
                test_00_draw,
                test_00_exit
        }, {
                "polyline",
                test_01_init,
                test_01_update,
                test_01_draw,
                test_01_exit
        }, {
                "normal-sprite",
                test_02_init,
                test_02_update,
                test_02_draw,
                test_02_exit
        }, {
                "scaled-sprite",
                test_03_init,
                test_03_update,
                test_03_draw,
                test_03_exit
        }, {
                "distorted-sprite",
                test_04_init,
                test_04_update,
                test_04_draw,
                test_04_exit
        }, {
                "line",
                test_05_init,
                test_05_update,
                test_05_draw,
                test_05_exit
        }, {
                "end-code",
                test_06_init,
                test_06_update,
                test_06_draw,
                test_06_exit
        }, {
                "color-calculations",
                NULL,
                NULL,
                NULL,
                NULL
        }, {
                "framebuffer",
                NULL,
                NULL,
                NULL,
                NULL
        }, {
                "resolutions",
                NULL,
                NULL,
                NULL,
                NULL
        }, {
                '\0',
                NULL,
                NULL,
                NULL,
                NULL
        }
};

int
main(void)
{
        hardware_init();

        cons_init(&cons, CONS_DRIVER_VDP2);

        test_init();

        int error;

        uint32_t test_idx;
        for (test_idx = 0; ; test_idx++) {
                struct test *test;
                test = &tests[test_idx];

                if (test->test_name == '\0') {
                        break;
                }

                error = test_register(test->test_name, test->test_init,
                    test->test_update, test->test_draw, test->test_exit);
                assert(error >= 0);
        }

        error = test_load("polygon");
        assert(error >= 0);

        while (true) {
                vdp2_tvmd_vblank_out_wait();
                test_update();
                vdp2_tvmd_vblank_in_wait();
                test_draw();
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
            VRAM_ADDR_4MBIT(2, 0x1FFFE), blcs_color, 0);
}

static void
vblank_in_handler(irq_mux_handle_t *irq_mux __unused)
{
        smpc_peripheral_digital_port(1, &digital_pad);
}

static void
vblank_out_handler(irq_mux_handle_t *irq_mux __unused)
{
        tick = (tick & 0xFFFFFFFF) + 1;
}

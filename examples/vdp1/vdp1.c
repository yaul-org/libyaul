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
#include "test.h"

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   224

extern uint8_t root_romdisk[];

static void vblank_in_handler(irq_mux_handle_t *);
static void vblank_out_handler(irq_mux_handle_t *);

static void hardware_init(void);


#define TESTS_CNT 9

#define TEST_DECLARE_PROTOTYPE(n)                                              \
    void CC_CONCAT(CC_CONCAT(test_, n),_init(void));                           \
    void CC_CONCAT(CC_CONCAT(test_, n),_update(void));                         \
    void CC_CONCAT(CC_CONCAT(test_, n),_draw(void));                           \
    void CC_CONCAT(CC_CONCAT(test_, n),_exit(void))

TEST_DECLARE_PROTOTYPE(00);
TEST_DECLARE_PROTOTYPE(01);
TEST_DECLARE_PROTOTYPE(02);
TEST_DECLARE_PROTOTYPE(03);
TEST_DECLARE_PROTOTYPE(04);
TEST_DECLARE_PROTOTYPE(05);
TEST_DECLARE_PROTOTYPE(06);
TEST_DECLARE_PROTOTYPE(07);
TEST_DECLARE_PROTOTYPE(08);

static struct test {
        char *name;
        void (*init)(void);
        void (*update)(void);
        void (*draw)(void);
        void (*exit)(void);
} tests[TESTS_CNT] = {
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
        }
};


#define MENU_BUTTONS_CNT 9

static struct menu_button {
        const char *button_texture_path;
        uint16_t button_width;
        uint16_t button_height;
} menu_buttons[MENU_BUTTONS_CNT] __unused = {
        {
                "/BUTTONS/POLYGON.TGA",
                128,
                32
        }, {
                "/BUTTONS/POLYLINE.TGA",
                128,
                32
        }, {
                "/BUTTONS/N_SPRITE.TGA",
                128,
                32
        }, {
                "/BUTTONS/S_SPRITE.TGA",
                128,
                32
        }, {
                "/BUTTONS/D_SPRITE.TGA",
                128,
                32
        }, {
                "/BUTTONS/LINE.TGA",
                128,
                32
        }, {
                "/BUTTONS/END_CODE.TGA",
                128,
                32
        }, {
                "/BUTTONS/FRAMEBUFFER.TGA",
                128,
                32
        }, {
                "/BUTTONS/RESOLUTION.TGA",
                128,
                32
        }
};


static struct state_data {
        int32_t current_test_idx;
        int32_t last_test_idx;
        int32_t current_button_idx;
        struct vdp1_cmdt_sprite button_sprites[MENU_BUTTONS_CNT];
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
        cons_init(&cons, CONS_DRIVER_VDP2);

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

        state_data.current_button_idx = 0;
        state_data.current_test_idx = -1;
        state_data.last_test_idx = -1;

        state_machine_transition(&state_vdp1, STATE_VDP1_MENU);

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

static void
state_00_init(struct state_context *state_context __unused)
{
        static struct {
                int16_t x;
                int16_t y;
        } button_positions[MENU_BUTTONS_CNT] = {
                {
                        16,
                        16
                }, {
                        16,
                        16 + 40
                }, {
                        16,
                        16 + 40 + 40
                }, {
                        16,
                        16 + 40 + 40 + 40
                }, {
                        16 + 100,
                        16
                }, {
                        16 + 100,
                        16 + 40
                }, {
                        16 + 100,
                        16 + 40 + 40
                }, {
                        16 + 100,
                        16 + 40 + 40 + 40
                }, {
                        16 + 200,
                        16
                }
        };

        static uint16_t blcs_color[SCREEN_HEIGHT] __unused;

        struct state_data *state_data;
        state_data = (struct state_data *)state_context->sc_data;

        uint32_t button_idx;
        for (button_idx = 0; button_idx < MENU_BUTTONS_CNT; button_idx++) {
                struct menu_button *button;
                button = &menu_buttons[button_idx];

                void *fh;
                fh = fs_open(button->button_texture_path);

                uint8_t *data_ptr;
                data_ptr = (uint8_t *)0x00201000;

                fs_read(fh, data_ptr);
                fs_close(fh);

                tga_t tga;
                int status;
                status = tga_read(&tga, data_ptr);
                assert(status == TGA_FILE_OK);

                uint16_t *vram_addr;
                uint32_t vram_bytes;
                vram_bytes = common_round_pow2(
                        tga.tga_width * tga.tga_height * (tga.tga_bpp / 8));
                vram_addr = (uint16_t *)CHAR(button_idx * vram_bytes);
                uint32_t amount;
                amount = tga_image_decode(&tga, (void *)vram_addr);
                assert(amount > 0);

                struct vdp1_cmdt_sprite *sprite;
                sprite = &state_data->button_sprites[button_idx];

                sprite->cs_type = CMDT_TYPE_NORMAL_SPRITE;
                sprite->cs_mode.cc_mode = 2;
                sprite->cs_mode.color_mode = 5;
                sprite->cs_color_bank = 0;
                sprite->cs_mode.transparent_pixel = true;
                sprite->cs_mode.high_speed_shrink = false;
                sprite->cs_char = (uint32_t)vram_addr;
                sprite->cs_position.x = button_positions[button_idx].x;
                sprite->cs_position.y = button_positions[button_idx].y;
                sprite->cs_width = button->button_width;
                sprite->cs_height = button->button_height;
        }

        vdp2_tvmd_display_clear();
        uint32_t color_idx;
        for (color_idx = 0; color_idx < SCREEN_HEIGHT; color_idx++) {
                blcs_color[color_idx] = 0x8000 | RGB888_TO_RGB555(
                        0,
                        (SCREEN_HEIGHT - color_idx),
                        0);
        }

        vdp2_scrn_back_screen_set(/* single_color = */ false,
            VRAM_ADDR_4MBIT(0, 0), blcs_color, SCREEN_HEIGHT);
        vdp2_tvmd_display_set();
}

static void
state_00_update(struct state_context *state_context)
{
        struct state_data *state_data __unused;
        state_data = (struct state_data *)state_context->sc_data;

        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.down) {
                        if (state_data->current_button_idx < (MENU_BUTTONS_CNT - 1)) {
                                state_data->current_button_idx++;
                        }
                } else if (digital_pad.held.button.up) {
                        if (state_data->current_button_idx > 0) {
                                state_data->current_button_idx--;
                        }
                } else if (digital_pad.held.button.a || digital_pad.held.button.c) {
                        state_data->last_test_idx = state_data->current_test_idx;
                        state_data->current_test_idx = state_data->current_button_idx;

                        state_machine_transition(&state_vdp1,
                            STATE_VDP1_TESTING);
                }
        }

        vdp1_cmdt_list_begin(0); {
                struct vdp1_cmdt_local_coord local_coord;
                local_coord.lc_coord.x = 0;
                local_coord.lc_coord.y = 0;

                struct vdp1_cmdt_system_clip_coord system_clip;
                system_clip.scc_coord.x = SCREEN_WIDTH - 1;
                system_clip.scc_coord.y = SCREEN_HEIGHT - 1;

                struct vdp1_cmdt_user_clip_coord user_clip;
                user_clip.ucc_coords[0].x = 0;
                user_clip.ucc_coords[0].y = 0;
                user_clip.ucc_coords[1].x = SCREEN_WIDTH - 1;
                user_clip.ucc_coords[1].y = SCREEN_HEIGHT - 1;

                vdp1_cmdt_system_clip_coord_set(&system_clip);
                vdp1_cmdt_user_clip_coord_set(&user_clip);
                vdp1_cmdt_local_coord_set(&local_coord);

                uint32_t button_idx;
                for (button_idx = 0; button_idx < MENU_BUTTONS_CNT; button_idx++) {
                        struct vdp1_cmdt_sprite *sprite;
                        sprite = &state_data->button_sprites[button_idx];
                        sprite->cs_mode.cc_mode = 2;
                        if (button_idx == (uint32_t)state_data->current_button_idx) {
                                sprite->cs_mode.cc_mode = 0;
                        }

                        vdp1_cmdt_sprite_draw(sprite);
                }

                local_coord.lc_coord.x = SCREEN_WIDTH / 2;
                local_coord.lc_coord.y = SCREEN_HEIGHT / 2;

                vdp1_cmdt_local_coord_set(&local_coord);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

static void
state_00_draw(struct state_context *state_context __unused)
{
        vdp1_cmdt_list_commit();
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

        struct test *current_test;
        current_test = &tests[state_data->current_test_idx];

        current_test->init();
}

static void
state_01_update(struct state_context *state_context)
{
        struct state_data *state_data;
        state_data = (struct state_data *)state_context->sc_data;

        struct test *current_test;
        current_test = &tests[state_data->current_test_idx];

        current_test->update();
}

static void
state_01_draw(struct state_context *state_context)
{
        struct state_data *state_data;
        state_data = (struct state_data *)state_context->sc_data;

        struct test *current_test;
        current_test = &tests[state_data->current_test_idx];

        current_test->draw();
}

static void
state_01_exit(struct state_context *state_context)
{
        struct state_data *state_data;
        state_data = (struct state_data *)state_context->sc_data;

        struct test *current_test;
        current_test = &tests[state_data->current_test_idx];

        current_test->exit();
}

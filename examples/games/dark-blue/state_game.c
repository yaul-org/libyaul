/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmd_groups.h"
#include "common.h"
#include "globals.h"
#include "objects.h"
#include "physics.h"
#include "state.h"
#include "fs.h"
#include "fs_texture.h"

#define GAME_STATE_FADE_IN_INIT   0
#define GAME_STATE_FADE_IN        1
#define GAME_STATE_FADE_IN_EXIT   2
#define GAME_STATE_READY_INIT     3
#define GAME_STATE_READY          4
#define GAME_STATE_READY_EXIT     5
#define GAME_STATE_GAME_INIT      6
#define GAME_STATE_GAME           7
#define GAME_STATE_GAME_EXIT      8

#define READY_STATE_IDLE          0
#define READY_STATE_TOP_ENTER     1
#define READY_STATE_MIDDLE_ENTER  2
#define READY_STATE_BOTTOM_ENTER  3
#define READY_STATE_WAIT          4
#define READY_STATE_TOP_LEAVE     5
#define READY_STATE_MIDDLE_LEAVE  6
#define READY_STATE_BOTTOM_LEAVE  7
#define READY_STATE_READY         8

static struct state_game {
        uint32_t sg_state;
} local_state_game;

static const char *game_state2str[] = {
        "GAME_STATE_FADE_IN_INIT",
        "GAME_STATE_FADE_IN",
        "GAME_STATE_FADE_IN_EXIT",
        "GAME_STATE_READY_INIT",
        "GAME_STATE_READY",
        "GAME_STATE_READY_EXIT",
        "GAME_STATE_GAME_INIT",
        "GAME_STATE_GAME",
        "GAME_STATE_GAME_EXIT"
};

static void ready_m_start(struct object *, fix16_t);
static bool ready_m_ready(struct object *);

struct object_ready {
        OBJECT_DECLARATIONS

        /* Public data */
        struct {
            void (*m_start)(struct object *, fix16_t);
            bool (*m_ready)(struct object *);
        } functions;

        struct {
        } data;

        /* Private data */
        struct {
                uint16_t m_state;
                uint16_t m_width;
                uint16_t m_height;
                fix16_t m_speed;
                struct cmd_group m_cmd_group;
                struct vdp1_cmdt_local_coord m_local_coord;
                struct vdp1_cmdt_sprite m_ns_top;
                struct vdp1_cmdt_sprite m_ns_middle;
                struct vdp1_cmdt_sprite m_ns_bottom;
        } private_data;
};

static void on_ready_init(struct object *);
static void on_ready_update(struct object *);
static void on_ready_draw(struct object *);

static struct object_ready object_ready = {
        .active = true,
        .on_init = on_ready_init,
        .rigid_body = NULL,
        .colliders = {
                NULL
        },
        .on_update = on_ready_update,
        .on_draw = on_ready_draw,
        .on_destroy = NULL,
        .on_collision = NULL,
        .on_trigger = NULL,
        .functions = {
            .m_start = ready_m_start,
            .m_ready = ready_m_ready
        }
};

void
state_game_init(struct state_context *state_context)
{
        /* Set game state */
        STATE_MACHINE_CONTEXT_DATA(state_context, game) = &local_state_game;

        struct state_game *state_game;
        state_game = STATE_MACHINE_CONTEXT_DATA(state_context, game);

        state_game->sg_state = GAME_STATE_FADE_IN_INIT;

        physics_init();
        cmd_groups_init();

        OBJECT_CALL_EVENT(&object_ready, init);
        OBJECT_CALL_EVENT(&object_fade, init);
        OBJECT_CALL_EVENT(&object_world, init);
        OBJECT_CALL_EVENT(&object_player, init);
        OBJECT_CALL_EVENT(&object_camera, init);
}

void
state_game_update(struct state_context *state_context)
{
        struct state_game *state_game;
        state_game = STATE_MACHINE_CONTEXT_DATA(state_context, game);

        cons_buffer("[H[2J");
        (void)sprintf(text,
            "scanline diff=%i\n"
            "game state=%s\n",
            (int)(end_scanline - start_scanline),
            game_state2str[state_game->sg_state]);
        cons_buffer(text);

        switch (state_game->sg_state) {
        case GAME_STATE_FADE_IN_INIT:
                OBJECT_CALL_PUBLIC_MEMBER(&object_fade, start,
                    /* fade_in = */ true, F16(2.0f));
                OBJECT_CALL_PUBLIC_MEMBER(&object_world, start);
                OBJECT_CALL_PUBLIC_MEMBER(&object_camera, start, F16(0.25f));

                state_game->sg_state = GAME_STATE_FADE_IN;
                break;
        case GAME_STATE_FADE_IN:
                /* Check until we're done fading in */
                if ((OBJECT_CALL_PUBLIC_MEMBER(&object_fade, faded))) {
                        state_game->sg_state = GAME_STATE_READY_INIT;
                }
                break;
        case GAME_STATE_FADE_IN_EXIT:
                break;
        case GAME_STATE_READY_INIT:
                OBJECT_CALL_PUBLIC_MEMBER(&object_ready, start, F16(6.0f));
                state_game->sg_state = GAME_STATE_READY;
                break;
        case GAME_STATE_READY:
                if ((OBJECT_CALL_PUBLIC_MEMBER(&object_ready, ready))) {
                        state_game->sg_state = GAME_STATE_GAME_INIT;
                }
                break;
        case GAME_STATE_READY_EXIT:
                break;
        case GAME_STATE_GAME_INIT:
                OBJECT_CALL_PUBLIC_MEMBER(&object_player, start);
                state_game->sg_state = GAME_STATE_GAME;
                break;
        case GAME_STATE_GAME:
                /* Check if player is dead */
                if ((OBJECT_CALL_PUBLIC_MEMBER(&object_player, dead))) {
                        /* Stop the camera */
                        OBJECT_CALL_PUBLIC_MEMBER(&object_camera, stop);
                        /* If not enough lives, go to game over screen */
                        /* state_game->sg_state = GAME_STATE_FADE_IN_INIT; */
                }
                break;
        case GAME_STATE_GAME_EXIT:
                break;
        }

        OBJECT_CALL_EVENT(&object_fade, update);
        OBJECT_CALL_EVENT(&object_ready, update);
        OBJECT_CALL_EVENT(&object_player, update);
        OBJECT_CALL_EVENT(&object_camera, update);
        OBJECT_CALL_EVENT(&object_world, update);

        physics_update();
        cmd_groups_update();
}

void
state_game_draw(struct state_context *state_context __unused)
{
        OBJECT_CALL_EVENT(&object_fade, draw);
        OBJECT_CALL_EVENT(&object_ready, draw);
        OBJECT_CALL_EVENT(&object_world, draw);
        OBJECT_CALL_EVENT(&object_player, draw);
        OBJECT_CALL_EVENT(&object_camera, draw);

        cons_flush();
        vdp1_cmdt_list_commit();
}

void
state_game_exit(struct state_context *state_context __unused)
{
}

static void
on_ready_init(struct object *this __unused)
{
        THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_IDLE;

        fs_texture_vdp1_load("/READY.TGA", (void *)CHAR(0),
            (uint16_t *)CLUT(16, 0));

        uint16_t width;
        width = 168;
        THIS_PRIVATE_DATA(object_ready, width) = width;

        uint16_t height;
        height = 12;
        THIS_PRIVATE_DATA(object_ready, height) = height;

        uint16_t offset;
        offset = SCREEN_WIDTH - (width / 2);

        THIS_PRIVATE_DATA(object_ready, local_coord).lc_coord.x = 0;
        THIS_PRIVATE_DATA(object_ready, local_coord).lc_coord.y = SCREEN_HEIGHT - 1;

        THIS_PRIVATE_DATA(object_ready, ns_top).cs_type = CMDT_TYPE_NORMAL_SPRITE;
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_mode.cc_mode = 0;
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_mode.color_mode = 1;
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_mode.transparent_pixel = true;
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_clut = CLUT(16, 0);
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_char = CHAR(0);
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_width = width;
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_height = height;
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_position.x = (width / 2) + offset;
        THIS_PRIVATE_DATA(object_ready, ns_top).cs_position.y = -SCREEN_HEIGHT / 2;

        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_type = CMDT_TYPE_NORMAL_SPRITE;
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_mode.cc_mode = 0;
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_mode.color_mode = 1;
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_mode.transparent_pixel = true;
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_clut = CLUT(16, 0);
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_char = CHAR((width * height) / 2);
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_width = width;
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_height = height;
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_position.x = (width / 2) + offset;
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_position.y = (-SCREEN_HEIGHT / 2) + height;

        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_type = CMDT_TYPE_NORMAL_SPRITE;
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_mode.cc_mode = 0;
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_mode.color_mode = 1;
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_mode.transparent_pixel = true;
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_clut = CLUT(16, 0);
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_char = CHAR((width * (height * 2)) / 2);
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_width = width;
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_height = height;
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_position.x = (width / 2) + offset;
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_position.y = (-SCREEN_HEIGHT / 2) + (height * 2);

        struct cmd_group *cmd_group;
        cmd_group = &THIS_PRIVATE_DATA(object_ready, cmd_group);
        cmd_group_init(cmd_group);
        cmd_group->priority = 0;
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_LOCAL_COORD,
            &THIS_PRIVATE_DATA(object_ready, local_coord));
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_NORMAL_SPRITE,
            &THIS_PRIVATE_DATA(object_ready, ns_top));
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_NORMAL_SPRITE,
            &THIS_PRIVATE_DATA(object_ready, ns_middle));
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_NORMAL_SPRITE,
            &THIS_PRIVATE_DATA(object_ready, ns_bottom));
}

static void
on_ready_update(struct object *this __unused)
{
        uint16_t width;
        width = THIS_PRIVATE_DATA(object_ready, width);

        int16_t speed;
        speed = fix16_to_int(THIS_PRIVATE_DATA(object_ready, speed));

        switch (THIS_PRIVATE_DATA(object_ready, state)) {
        case READY_STATE_IDLE:
                break;
        case READY_STATE_TOP_ENTER:
                THIS_PRIVATE_DATA(object_ready, ns_top).cs_position.x -= speed;
                if (THIS_PRIVATE_DATA(object_ready, ns_top).cs_position.x <= (width / 2)) {
                    THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_MIDDLE_ENTER;
                }
                break;
        case READY_STATE_MIDDLE_ENTER:
                THIS_PRIVATE_DATA(object_ready, ns_middle).cs_position.x -= speed;
                if (THIS_PRIVATE_DATA(object_ready, ns_middle).cs_position.x <= (width / 2)) {
                    THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_BOTTOM_ENTER;
                }
                break;
        case READY_STATE_BOTTOM_ENTER:
                THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_position.x -= speed;
                if (THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_position.x <= (width / 2)) {
                    THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_WAIT;
                }
                break;
        case READY_STATE_WAIT:
                THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_TOP_LEAVE;
                break;
        case READY_STATE_TOP_LEAVE:
                THIS_PRIVATE_DATA(object_ready, ns_top).cs_position.x -= speed;
                if (THIS_PRIVATE_DATA(object_ready, ns_top).cs_position.x <= -width) {
                    THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_MIDDLE_LEAVE;
                }
                break;
        case READY_STATE_MIDDLE_LEAVE:
                THIS_PRIVATE_DATA(object_ready, ns_middle).cs_position.x -= speed;
                if (THIS_PRIVATE_DATA(object_ready, ns_middle).cs_position.x <= -width) {
                    THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_BOTTOM_LEAVE;
                }
                break;
        case READY_STATE_BOTTOM_LEAVE:
                THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_position.x -= speed;
                if (THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_position.x <= -width) {
                    THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_READY;
                }
                break;
        case READY_STATE_READY:
                break;
        }

        cmd_groups_add(&THIS_PRIVATE_DATA(object_ready, cmd_group));
}

static void
on_ready_draw(struct object *this __unused)
{
}

static void
ready_m_start(struct object *this, fix16_t speed)
{
        THIS_PRIVATE_DATA(object_ready, speed) = speed;
        THIS_PRIVATE_DATA(object_ready, state) = READY_STATE_TOP_ENTER;

        uint16_t width;
        width = THIS_PRIVATE_DATA(object_ready, width);
        uint16_t offset;
        offset = SCREEN_WIDTH - (width / 2);

        THIS_PRIVATE_DATA(object_ready, ns_top).cs_position.x = (width / 2) + offset;
        THIS_PRIVATE_DATA(object_ready, ns_middle).cs_position.x = (width / 2) + offset;
        THIS_PRIVATE_DATA(object_ready, ns_bottom).cs_position.x = (width / 2) + offset;
}

static bool
ready_m_ready(struct object *this __unused)
{
        return THIS_PRIVATE_DATA(object_ready, state) == READY_STATE_READY;
}

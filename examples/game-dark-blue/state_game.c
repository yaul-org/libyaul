/*
 * Copyright (c) 2012-2014 Israel Jacquez
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

#include "common.h"
#include "globals.h"
#include "objects.h"
#include "physics.h"
#include "state.h"

void
state_game_init(struct state_context *state_context __unused)
{
        physics_init();

        OBJECT_CALL_EVENT(&object_world, init);
        OBJECT_CALL_EVENT(&object_player, init);
        OBJECT_CALL_EVENT(&object_camera, init);
}

void
state_game_update(struct state_context *state_context)
{
        struct state_data *state_data __unused;
        state_data = (struct state_data *)state_context->sc_data;

        cons_buffer(&cons, "[H[2J");

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_system_clip_coord_set(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
                vdp1_cmdt_user_clip_coord_set(0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
                vdp1_cmdt_local_coord_set(0, SCREEN_HEIGHT - 1);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);

        int16_vector2_t old_position;
        old_position.x = object_player.transform.position.x;
        old_position.y = object_player.transform.position.y;

        int16_vector2_t new_position;
        memcpy(&new_position, &old_position, sizeof(int16_vector2_t));

        if (digital_pad.connected == 1) {
                if (digital_pad.pressed.button.left) {
                        object_player.private_data.m_direction = -1;
                        object_player.private_data.m_state = PLAYER_STATE_RUN;

                        new_position.x = object_player.transform.position.x +
                            object_player.private_data.m_direction;
                } else if (digital_pad.pressed.button.right) {
                        object_player.private_data.m_direction = 1;
                        object_player.private_data.m_state = PLAYER_STATE_RUN;

                        new_position.x = object_player.transform.position.x +
                            object_player.private_data.m_direction;
                } else {
                        object_player.private_data.m_state = PLAYER_STATE_IDLE;
                }

                if (digital_pad.pressed.button.up) {
                        object_player.private_data.m_state = PLAYER_STATE_RUN;

                        new_position.y = object_player.transform.position.y + 1;
                } else if (digital_pad.pressed.button.down) {
                        object_player.private_data.m_state = PLAYER_STATE_RUN;

                        new_position.y = object_player.transform.position.y - 1;
                }
        }

        object_player.transform.position.x = new_position.x;
        object_player.transform.position.y = new_position.y;

        OBJECT_CALL_EVENT(&object_player, update);
        OBJECT_CALL_EVENT(&object_camera, update);
        OBJECT_CALL_EVENT(&object_world, update);

        physics_update();
}

void
state_game_draw(struct state_context *state_context __unused)
{
        OBJECT_CALL_EVENT(&object_world, draw);
        OBJECT_CALL_EVENT(&object_player, draw);
        OBJECT_CALL_EVENT(&object_camera, draw);

        cons_flush(&cons);
        vdp1_cmdt_list_commit();
}

void
state_game_exit(struct state_context *state_context __unused)
{
}

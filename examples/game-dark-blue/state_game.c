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

#include "cmd_groups.h"
#include "common.h"
#include "globals.h"
#include "objects.h"
#include "physics.h"
#include "state.h"

void
state_game_init(struct state_context *state_context __unused)
{
        physics_init();
        cmd_groups_init();

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

        OBJECT_CALL_EVENT(&object_player, update);
        OBJECT_CALL_EVENT(&object_camera, update);
        OBJECT_CALL_EVENT(&object_world, update);

        physics_update();
        cmd_groups_update();
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

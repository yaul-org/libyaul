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
#include "state.h"

static struct state_splash {
} local_state_splash;

void
state_splash_init(struct state_context *state_context __unused)
{
        /* Set splash state */
        ((struct state_data *)state_context->sc_data)->state_splash = &local_state_splash;

        struct state_splash *state_splash __unused;
        state_splash = ((struct state_data *)state_context->sc_data)->state_splash;
}

void
state_splash_update(struct state_context *state_context)
{
        struct state_splash *state_splash __unused;
        state_splash = ((struct state_data *)state_context->sc_data)->state_splash;
}

void
state_splash_draw(struct state_context *state_context)
{
        struct state_splash *state_splash __unused;
        state_splash = ((struct state_data *)state_context->sc_data)->state_splash;
}

void
state_splash_exit(struct state_context *state_context)
{
        struct state_splash *state_splash __unused;
        state_splash = ((struct state_data *)state_context->sc_data)->state_splash;
}

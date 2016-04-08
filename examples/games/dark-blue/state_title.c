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

#define STATE_TITLE_STATE_LOGO_ANIMATE_INIT             0
#define STATE_TITLE_STATE_LOGO_ANIMATE                  1
#define STATE_TITLE_STATE_START_BUTTON_ANIMATE_INIT     2
#define STATE_TITLE_STATE_START_BUTTON_ANIMATE          3
#define STATE_TITLE_STATE_START_BUTTON_WAIT_INIT        4
#define STATE_TITLE_STATE_START_BUTTON_WAIT             5
#define STATE_TITLE_STATE_GAME                          6

static struct state_title {
        uint32_t st_state;
        struct scrn_cell_format st_nbg0_format;
        struct scrn_cell_format st_nbg1_format;
} local_state_title;

void
state_title_init(struct state_context *state_context __unused)
{
        /* Set title state */
        ((struct state_data *)state_context->sc_data)->state_title = &local_state_title;

        struct state_title *state_title;
        state_title = ((struct state_data *)state_context->sc_data)->state_title;

        state_title->st_state = STATE_TITLE_STATE_LOGO_ANIMATE_INIT;

        /* Set up X/Y and zoom factor value LUT */

        /* Set up NBG0 */
        /* Set up NBG1 */
}

void
state_title_update(struct state_context *state_context)
{
        struct state_title *state_title;
        state_title = ((struct state_data *)state_context->sc_data)->state_title;

        switch (state_title->st_state) {
        case STATE_TITLE_STATE_LOGO_ANIMATE_INIT:
                break;
        case STATE_TITLE_STATE_LOGO_ANIMATE:
                break;
        case STATE_TITLE_STATE_START_BUTTON_ANIMATE_INIT:
                break;
        case STATE_TITLE_STATE_START_BUTTON_ANIMATE:
                break;
        case STATE_TITLE_STATE_START_BUTTON_WAIT_INIT:
                break;
        case STATE_TITLE_STATE_START_BUTTON_WAIT:
                break;
        case STATE_TITLE_STATE_GAME:
                break;
        }
}

void
state_title_draw(struct state_context *state_context)
{
        struct state_title *state_title __unused;
        state_title = ((struct state_data *)state_context->sc_data)->state_title;
}

void
state_title_exit(struct state_context *state_context)
{
        struct state_title *state_title __unused;
        state_title = ((struct state_data *)state_context->sc_data)->state_title;
}

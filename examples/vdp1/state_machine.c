/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "state_machine.h"

void
state_machine_init(struct state_machine *state_machine)
{
        assert(state_machine != NULL);

        TAILQ_INIT(&state_machine->sm_states);

        state_machine->sm_prev_state = NULL;
        state_machine->sm_curr_state = NULL;
}

void
state_machine_add_state(struct state_machine *state_machine,
    const char *state_name,
    uint32_t state_id,
    void (*state_init)(struct state_context *),
    void (*state_update)(struct state_context *),
    void (*state_draw)(struct state_context *),
    void (*state_exit)(struct state_context *))
{
        struct state *state;
        state = (struct state *)malloc(sizeof(struct state));
        assert(state != NULL);

        state->state_name = state_name;
        state->state_id = state_id;
        state->state_init = state_init;
        state->state_update = state_update;
        state->state_draw = state_draw;
        state->state_exit = state_exit;

        /* Initialize state context */
        state->state_context.sc_frames = 0;

        TAILQ_INSERT_TAIL(&state_machine->sm_states, state, entries);
}

void
state_machine_transition(struct state_machine *state_machine, uint32_t state_id)
{
        state_machine->sm_prev_state = state_machine->sm_curr_state;

        struct state *prev_state;
        prev_state = state_machine->sm_prev_state;
        if (prev_state != NULL) {
                if (prev_state->state_exit != NULL) {
                        prev_state->state_exit(&prev_state->state_context);
                }

                prev_state->state_context.sc_frames = 0;
        }

        /* Check if transitioning state is valid */
        struct state *state;

        bool state_found;
        state_found = false;
        TAILQ_FOREACH (state, &state_machine->sm_states, entries) {
                if (state->state_id == state_id) {
                        state_found = true;
                        break;
                }
        }
        assert(state_found);
        state_machine->sm_curr_state = state;

        struct state *curr_state;
        curr_state = state_machine->sm_curr_state;

        if (curr_state->state_init != NULL) {
                curr_state->state_init(&curr_state->state_context);
        }
}

void
state_machine_handler_update(struct state_machine *state_machine)
{
        struct state *state;
        state = state_machine->sm_curr_state;

        state->state_context.sc_frames++;

        if (state->state_update != NULL) {
                state->state_update(&state->state_context);
        }
}

void
state_machine_handler_draw(struct state_machine *state_machine)
{
        struct state *state;
        state = state_machine->sm_curr_state;

        if (state->state_context.sc_frames == 0) {
                return;
        }

        if (state->state_draw != NULL) {
                state->state_draw(&state->state_context);
        }
}

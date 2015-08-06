/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <assert.h>
#include <inttypes.h>

#include <sys/queue.h>

TAILQ_HEAD(states, state);

struct state_context {
        uint32_t sc_frames;
        void *sc_data;
};

struct state {
        struct state_context state_context;

        const char *state_name;

        uint32_t state_id;

        void (*state_init)(struct state_context *);
        void (*state_update)(struct state_context *);
        void (*state_draw)(struct state_context *);
        void (*state_exit)(struct state_context *);

        TAILQ_ENTRY(state) entries;
};

struct state_machine {
        struct states sm_states;

        struct state *sm_prev_state;
        struct state *sm_curr_state;
};

void state_machine_init(struct state_machine *);
void state_machine_transition(struct state_machine *, uint32_t);
void state_machine_handler_update(struct state_machine *);
void state_machine_handler_draw(struct state_machine *);

void state_machine_add_state(struct state_machine *, const char *, uint32_t,
    void (*)(struct state_context *),
    void (*)(struct state_context *),
    void (*)(struct state_context *),
    void (*)(struct state_context *),
    void *);

#endif /* !STATE_MACHINE_H */

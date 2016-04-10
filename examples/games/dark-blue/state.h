/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef STATE_H
#define STATE_H

#include "state_machine.h"

#define STATE_GAME_SPLASH       0
#define STATE_GAME_TITLE        1
#define STATE_GAME_GAME         2
#define STATE_GAME_GAME_OVER    3

/* Each state in the main state machine has a pointer to its own state
 * data. It's up to the state to populate/use the state data. */
struct state_data {
        void *state_splash;
        void *state_title;
        void *state_game;
        void *state_game_over;
};

void state_splash_init(struct state_context *);
void state_splash_update(struct state_context *);
void state_splash_draw(struct state_context *);
void state_splash_exit(struct state_context *);

void state_title_init(struct state_context *);
void state_title_update(struct state_context *);
void state_title_draw(struct state_context *);
void state_title_exit(struct state_context *);

void state_game_init(struct state_context *);
void state_game_update(struct state_context *);
void state_game_draw(struct state_context *);
void state_game_exit(struct state_context *);

void state_game_over_init(struct state_context *);
void state_game_over_update(struct state_context *);
void state_game_over_draw(struct state_context *);
void state_game_over_exit(struct state_context *);

extern struct state_machine state_game;

#endif /* !STATE_H */

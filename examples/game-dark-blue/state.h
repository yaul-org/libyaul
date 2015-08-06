/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef STATE_H
#define STATE_H

#include "state_machine.h"

#define STATE_GAME_TITLE        0
#define STATE_GAME_GAME         1

struct state_data {
};

void state_game_init(struct state_context *);
void state_game_update(struct state_context *);
void state_game_draw(struct state_context *);
void state_game_exit(struct state_context *);

extern struct state_machine state_game;

#endif /* !STATE_H */

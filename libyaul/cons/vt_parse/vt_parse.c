/*
 * Copyright (c) 2012 Joshua Haberman
 * See LICENSE for details.
 *
 * Joshua Haberman <joshua@reverberate.org>
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "vt_parse.h"

void
vt_parse_init(vt_parse_t *parser, vt_parse_callback_t cb, void *user_data)
{

        parser->state = VT_PARSE_STATE_GROUND;
        parser->intermediate_chars[0] = '\0';
        parser->num_params = 0;
        parser->ignore_flagged = 0;
        parser->cb = cb;
        parser->user_data = user_data;
}

static
void do_action(vt_parse_t *parser, vt_parse_action_t action, char ch)
{
        int num_intermediate_chars;

        /* Some actions we handle internally (like parsing parameters), others
         * we hand to our client for processing */

        switch (action) {
        case VT_PARSE_ACTION_PRINT:
        case VT_PARSE_ACTION_EXECUTE:
        case VT_PARSE_ACTION_HOOK:
        case VT_PARSE_ACTION_PUT:
        case VT_PARSE_ACTION_OSC_START:
        case VT_PARSE_ACTION_OSC_PUT:
        case VT_PARSE_ACTION_OSC_END:
        case VT_PARSE_ACTION_UNHOOK:
        case VT_PARSE_ACTION_CSI_DISPATCH:
        case VT_PARSE_ACTION_ESC_DISPATCH:
                parser->cb(parser, action, ch);
                break;
        case VT_PARSE_ACTION_IGNORE:
                /* Do nothing */
                break;
        case VT_PARSE_ACTION_COLLECT:
                /* Append the character to the intermediate params */
                num_intermediate_chars = strlen((char *)parser->intermediate_chars);
                if (num_intermediate_chars + 1 > MAX_INTERMEDIATE_CHARS)
                        parser->ignore_flagged = 1;
                else
                        parser->intermediate_chars[num_intermediate_chars++] = ch;
                break;
        case VT_PARSE_ACTION_PARAM:
                /* Process the param character */
                if (ch == ';') {
                        parser->num_params++;
                        parser->params[parser->num_params - 1] = 0;
                } else {
                        /* The character is a digit */
                        int current_param;

                        if (parser->num_params == 0) {
                                parser->num_params = 1;
                                parser->params[0] = 0;
                        }

                        current_param = parser->num_params - 1;
                        parser->params[current_param] *= 10;
                        parser->params[current_param] += (ch - '0');
                }
                break;
        case VT_PARSE_ACTION_CLEAR:
                parser->intermediate_chars[0] = '\0';
                parser->num_params = 0;
                parser->ignore_flagged = 0;
                break;
        default:
                fprintf(stderr, "Internal error, unknown action %d", action);
        }
}

static void
do_state_change(vt_parse_t *parser, state_change_t change, char ch)
{
        /* A state change is an action and/or a new state to transition to */
        vt_parse_state_t  new_state = STATE(change);
        vt_parse_action_t action = ACTION(change);

        if (new_state) {
                /*
                 * Perform up to three actions:
                 *   1. the exit action of the old state
                 *   2. the action associated with the transition
                 *   3. the entry actionk of the new action
                 */

                vt_parse_action_t exit_action;
                vt_parse_action_t entry_action;

                exit_action = exit_actions[parser->state];
                entry_action = entry_actions[new_state];

                if (exit_action)
                        do_action(parser, exit_action, 0);

                if (action)
                        do_action(parser, action, ch);

                if (entry_action)
                        do_action(parser, entry_action, 0);

                parser->state = new_state;
        } else
                do_action(parser, action, ch);
}

void
vt_parse(vt_parse_t *parser, const char *data, int len)
{
        int i;
        uint8_t ch;
        state_change_t change;

        for (i = 0; i < len; i++) {
                ch = data[i];

                /* If a transition is defined from the "anywhere" state,
                 * always use that.  Otherwise use the transition from
                 * the current state */

                change = state_table[VT_PARSE_STATE_ANYWHERE][ch];
                if (!change)
                        change = state_table[parser->state][ch];
                do_state_change(parser, change, data[i]);
        }
}

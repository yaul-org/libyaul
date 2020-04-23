/* -*- mode: c -*- */

/*
 * Copyright (c) 2012 Joshua Haberman
 * See LICENSE for details.
 *
 * Joshua Haberman <joshua@reverberate.org>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INTERMEDIATE_CHARS  2

#define ACTION(sc)      ((sc) & 0x0F)
#define STATE(sc)       ((sc) >> 4)

typedef enum {
        VT_PARSE_STATE_ANYWHERE = 0,
        VT_PARSE_STATE_CSI_ENTRY = 1,
        VT_PARSE_STATE_CSI_IGNORE = 2,
        VT_PARSE_STATE_CSI_INTERMEDIATE = 3,
        VT_PARSE_STATE_CSI_PARAM = 4,
        VT_PARSE_STATE_DCS_ENTRY = 5,
        VT_PARSE_STATE_DCS_IGNORE = 6,
        VT_PARSE_STATE_DCS_INTERMEDIATE = 7,
        VT_PARSE_STATE_DCS_PARAM = 8,
        VT_PARSE_STATE_DCS_PASSTHROUGH = 9,
        VT_PARSE_STATE_ESCAPE = 10,
        VT_PARSE_STATE_ESCAPE_INTERMEDIATE = 11,
        VT_PARSE_STATE_GROUND = 12,
        VT_PARSE_STATE_OSC_STRING = 13,
        VT_PARSE_STATE_SOS_PM_APC_STRING = 14
} __packed vt_parse_state_t;

typedef enum {
        VT_PARSE_ACTION_CLEAR = 1,
        VT_PARSE_ACTION_COLLECT = 2,
        VT_PARSE_ACTION_CSI_DISPATCH = 3,
        VT_PARSE_ACTION_ESC_DISPATCH = 4,
        VT_PARSE_ACTION_EXECUTE = 5,
        VT_PARSE_ACTION_HOOK = 6,
        VT_PARSE_ACTION_IGNORE = 7,
        VT_PARSE_ACTION_OSC_END = 8,
        VT_PARSE_ACTION_OSC_PUT = 9,
        VT_PARSE_ACTION_OSC_START = 10,
        VT_PARSE_ACTION_PARAM = 11,
        VT_PARSE_ACTION_PRINT = 12,
        VT_PARSE_ACTION_PUT = 13,
        VT_PARSE_ACTION_UNHOOK = 14
} __packed vt_parse_action_t;

typedef uint8_t state_change_t;

struct vt_parse;

typedef void (*vt_parse_callback)(struct vt_parse *, vt_parse_action_t, int);

typedef struct vt_parse {
        vt_parse_state_t state;
        vt_parse_callback callback;
        uint8_t intermediate_chars[MAX_INTERMEDIATE_CHARS + 1];
        uint8_t ignore_flagged;
        uint8_t params[16];
        uint8_t num_params;
} __aligned(32) vt_parse_t;

static void _do_action(vt_parse_t *, vt_parse_action_t, char);
static void _do_state_change(vt_parse_t *, state_change_t, char);

#include "vt_parse_table.inc"

static void
_vt_parse_init(vt_parse_t *parser, vt_parse_callback callback)
{
        parser->state = VT_PARSE_STATE_GROUND;
        parser->intermediate_chars[0] = '\0';
        parser->num_params = 0;
        parser->ignore_flagged = 0;
        parser->callback = callback;
}

static void
_vt_parse(vt_parse_t *parser, const char *data, int32_t len)
{
        int32_t i;
        uint8_t ch;
        state_change_t change;

        for (i = 0; i < len; i++) {
                ch = data[i];

                /* If a transition is defined from the "anywhere" state,
                 * always use that.  Otherwise use the transition from
                 * the current state */

                change = _vt_parse_state_table[VT_PARSE_STATE_ANYWHERE][ch];

                if (!change) {
                        change = _vt_parse_state_table[parser->state][ch];
                }

                _do_state_change(parser, change, data[i]);
        }
}

static
void _do_action(vt_parse_t *parser, vt_parse_action_t action, char ch)
{
        int32_t num_intermediate_chars;

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
                parser->callback(parser, action, ch);
                break;
        case VT_PARSE_ACTION_IGNORE:
                /* Do nothing */
                break;
        case VT_PARSE_ACTION_COLLECT:
                /* Append the character to the intermediate params */
                num_intermediate_chars =
                    strlen((char *)parser->intermediate_chars);
                if ((num_intermediate_chars + 1) > MAX_INTERMEDIATE_CHARS) {
                        parser->ignore_flagged = 1;
                } else {
                        parser->intermediate_chars[num_intermediate_chars++] = ch;
                }
                break;
        case VT_PARSE_ACTION_PARAM:
                /* Process the param character */
                if (ch == ';') {
                        parser->num_params++;
                        parser->params[parser->num_params - 1] = 0;
                } else {
                        /* The character is a digit */
                        int32_t current_param;

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
                assert(false);
                return;
        }
}

static void
_do_state_change(vt_parse_t *parser, state_change_t change, char ch)
{
        /* A state change is an action and/or a new state to transition to */
        vt_parse_state_t new_state;
        new_state = STATE(change);

        vt_parse_action_t action;
        action = ACTION(change);

        if (new_state) {
                /* Perform up to three actions:
                 *   1. the exit action of the old state
                 *   2. the action associated with the transition
                 *   3. the entry actionk of the new action */

                vt_parse_action_t exit_action;
                vt_parse_action_t entry_action;

                exit_action = _vt_parse_exit_actions[parser->state];
                entry_action = _vt_parse_entry_actions[new_state];

                if (exit_action) {
                        _do_action(parser, exit_action, 0);
                }

                if (action) {
                        _do_action(parser, action, ch);
                }

                if (entry_action) {
                        _do_action(parser, entry_action, 0);
                }

                parser->state = new_state;
        } else {
                _do_action(parser, action, ch);
        }
}

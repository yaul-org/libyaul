/*
 * Copyright (c) 2012 Joshua Haberman
 * See LICENSE for details.
 *
 * Joshua Haberman <joshua@reverberate.org>
 */

#include <inttypes.h>

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
} vt_parse_state_t;

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
} vt_parse_action_t;

typedef uint8_t state_change_t;

extern state_change_t STATE_TABLE[15][256];
extern vt_parse_action_t ENTRY_ACTIONS[15];
extern vt_parse_action_t EXIT_ACTIONS[15];
extern char *ACTION_NAMES[15];
extern char *STATE_NAMES[15];

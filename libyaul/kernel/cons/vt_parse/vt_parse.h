/*
 * Copyright (c) 2012 Joshua Haberman
 * See LICENSE for details.
 *
 * Joshua Haberman <joshua@reverberate.org>
 */

#ifndef _VT_PARSE_H_
#define _VT_PARSE_H_

#include "vt_parse_table.h"

#define MAX_INTERMEDIATE_CHARS  2
#define ACTION(sc)              ((sc) & 0x0F)
#define STATE(sc)               ((sc) >> 4)

struct vt_parse;

typedef void (*vt_parse_callback_t)(struct vt_parse *, vt_parse_action_t, int);

typedef struct vt_parse {
        vt_parse_state_t state;
        vt_parse_callback_t cb;
        uint8_t intermediate_chars[MAX_INTERMEDIATE_CHARS + 1];
        char ignore_flagged;
        int params[16];
        int num_params;
} vt_parse_t;

void vt_parse_init(vt_parse_t *, vt_parse_callback_t);
void vt_parse(vt_parse_t *, const char *, int);

#endif /* !_VT_PARSE_H_ */

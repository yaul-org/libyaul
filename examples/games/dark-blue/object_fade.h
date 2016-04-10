/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef OBJECT_FADE_H
#define OBJECT_FADE_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include "bounding_box.h"
#include "cmd_groups.h"
#include "common.h"
#include "globals.h"
#include "object.h"
#include "state.h"

struct object_fade {
        OBJECT_DECLARATIONS

        /* Public data */
        struct {
            void (*m_start)(struct object *, bool, fix16_t);
            bool (*m_faded)(struct object *);
        } functions;

        struct {
        } data;

        /* Private data */
        struct {
            uint16_t m_state;
            bool m_fade_in;
            fix16_t m_speed;
            fix16_t m_fade;
        } private_data;
};

extern struct object_fade object_fade;

#endif /* !OBJECT_FADE_H */

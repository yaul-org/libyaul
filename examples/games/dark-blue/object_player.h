/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef OBJECT_PLAYER_H
#define OBJECT_PLAYER_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include "bounding_box.h"
#include "cmd_groups.h"
#include "common.h"
#include "globals.h"
#include "object.h"
#include "state.h"

struct object_player {
        OBJECT_DECLARATIONS

        /* Public data */
        struct {
            void (*m_start)(struct object *);
            bool (*m_dead)(struct object *);
        } functions;

        struct {
        } data;

        /* Private data */
        struct {
                void (*m_update_polygon)(struct object *);
                void (*m_update_input)(struct object *);
        } private_functions;

        struct {
                uint16_t m_width;
                uint16_t m_height;

                uint32_t m_state;
                uint32_t m_last_state;

                struct cmd_group m_cmd_group;
                struct vdp1_cmdt_polygon m_polygon;
        } private_data;
};

extern struct object_player object_player;

#endif /* !OBJECT_PLAYER_H */

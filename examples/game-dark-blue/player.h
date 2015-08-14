/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef PLAYER_H
#define PLAYER_H

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
#define PLAYER_COLOR                    RGB888_TO_RGB555(255, 255, 0)

#define PLAYER_STATE_IDLE               0
#define PLAYER_STATE_RUN                1
#define PLAYER_STATE_JUMP               2
#define PLAYER_STATE_DEAD               3

        OBJECT_DECLARATIONS

        struct {
                int16_t m_direction;

                uint16_t m_width;
                uint16_t m_height;

                uint32_t m_state;
                uint32_t m_last_state;

                struct cmd_group m_cmd_group;
                struct vdp1_cmdt_polygon m_polygon;
        } private_data;
};

extern struct object_player object_player;

#endif /* !PLAYER_H */

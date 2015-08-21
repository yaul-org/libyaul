/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef CAMERA_H
#define CAMERA_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include "bounding_box.h"
#include "cmd_groups.h"
#include "common.h"
#include "globals.h"
#include "object.h"
#include "state.h"

struct object_camera {
        OBJECT_DECLARATIONS

        struct {
                uint16_t m_median;
                uint16_t m_buffer;

                uint16_t m_last_state;
                uint16_t m_state;

                struct bounding_box m_lbb;
                struct bounding_box m_rbb;

                struct cmd_group m_cmd_group;
                struct vdp1_cmdt_line m_debug_line;
                struct vdp1_cmdt_polygon m_debug_polygon[2];
        } private_data;
};

extern struct object_camera object_camera;

#endif /* !CAMERA_H */

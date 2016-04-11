/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef OBJECT_CAMERA_H
#define OBJECT_CAMERA_H

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

        /* Public data */
        struct {
            void (*m_start)(struct object *, fix16_t);
            void (*m_stop)(struct object *);
        } functions;

        struct {
        } data;

        /* Private data */
        struct {
                uint16_t m_state;
                fix16_t m_speed;
                uint16_t m_median;
                struct cmd_group m_cmd_group;
                struct vdp1_cmdt_local_coord m_debug_local_coord;
                struct vdp1_cmdt_line m_debug_line;
        } private_data;
};

extern struct object_camera object_camera;

#endif /* !OBJECT_CAMERA_H */

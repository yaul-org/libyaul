/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include "objects.h"

#define CAMERA_STATE_IDLE       0
#define CAMERA_STATE_GO         1

static const char *camera_state2str[] = {
        "CAMERA_STATE_IDLE",
        "CAMERA_STATE_GO"
};

static void on_init(struct object *);
static void on_update(struct object *);
static void on_draw(struct object *);

static void m_start(struct object *, fix16_t);

struct object_camera object_camera = {
        .active = true,
        .id = OBJECT_CAMERA_ID,
        .rigid_body = NULL,
        .colliders = {
                NULL
        },
        .on_init = on_init,
        .on_update = on_update,
        .on_draw = on_draw,
        .on_destroy = NULL,
        .on_collision = NULL,
        .on_trigger = NULL,
        .functions = {
                .m_start = m_start
        }
};

static void
on_init(struct object *this)
{
        THIS(object_camera, transform).pos_int.x = 0;
        THIS(object_camera, transform).pos_int.y = 0;

        THIS(object_camera, transform).pos_fixed.x = F16(0.0f);
        THIS(object_camera, transform).pos_fixed.y = F16(0.0f);

        THIS_PRIVATE_DATA(object_camera, median) = SCREEN_WIDTH / 2;
        THIS_PRIVATE_DATA(object_camera, state) = CAMERA_STATE_IDLE;
        THIS_PRIVATE_DATA(object_camera, speed) = F16(0.0f);

        struct vdp1_cmdt_local_coord *debug_local_coord;
        debug_local_coord = &THIS_PRIVATE_DATA(object_camera, debug_local_coord);

        debug_local_coord->lc_coord.x = 0;
        debug_local_coord->lc_coord.y = SCREEN_HEIGHT - 1;

        struct vdp1_cmdt_line *debug_line;
        debug_line = &THIS_PRIVATE_DATA(object_camera, debug_line);

        debug_line->cl_color = RGB888_TO_RGB555(255,   0, 255);
        debug_line->cl_mode.transparent_pixel = true;
        debug_line->cl_mode.end_code = true;
        debug_line->cl_vertex.a.x = THIS_PRIVATE_DATA(object_camera, median);
        debug_line->cl_vertex.a.y = -SCREEN_HEIGHT;
        debug_line->cl_vertex.b.x = THIS_PRIVATE_DATA(object_camera, median);
        debug_line->cl_vertex.b.y = 0;

        struct cmd_group *cmd_group;
        cmd_group = &THIS_PRIVATE_DATA(object_camera, cmd_group);

        cmd_group_init(cmd_group);

        cmd_group->priority = 0;

        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_LOCAL_COORD,
            debug_local_coord);
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_LINE, debug_line);
}

static void
on_update(struct object *this)
{
        fix16_t old_position;
        uint16_t position;

        (void)sprintf(text,
            "camera state=%s\n",
            camera_state2str[THIS_PRIVATE_DATA(object_camera, state)]);
        cons_buffer(text);

        switch (THIS_PRIVATE_DATA(object_camera, state)) {
        case CAMERA_STATE_IDLE:
                break;
        case CAMERA_STATE_GO:
                old_position = THIS(object_camera, transform).pos_fixed.x;

                THIS(object_camera, transform).pos_fixed.x = fix16_add(old_position,
                    THIS_PRIVATE_DATA(object_camera, speed));


                position = fix16_to_int(THIS(object_camera, transform).pos_fixed.x);
                THIS(object_camera, transform).pos_int.x = position;
                break;
        }

        cmd_groups_add(&THIS_PRIVATE_DATA(object_camera, cmd_group));
}

static void
on_draw(struct object *this __unused)
{
}

static void
m_start(struct object *this, fix16_t speed)
{
        THIS_PRIVATE_DATA(object_camera, state) = CAMERA_STATE_GO;
        THIS_PRIVATE_DATA(object_camera, speed) = speed;

        THIS(object_camera, transform).pos_int.x = 0;
        THIS(object_camera, transform).pos_int.y = 0;

        THIS(object_camera, transform).pos_fixed.x = F16(0.0f);
        THIS(object_camera, transform).pos_fixed.y = F16(0.0f);
}

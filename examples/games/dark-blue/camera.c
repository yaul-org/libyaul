/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include "objects.h"

#define STATE_INVALID           -1
#define STATE_ADJUST_BB         0
#define STATE_IDLE              1
#define STATE_LEFT              2
#define STATE_FIRST_LEFT        3
#define STATE_FIRST_RIGHT       4
#define STATE_RIGHT             5

static void on_init(struct object *);
static void on_update(struct object *);
static void on_draw(struct object *);

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
        .on_trigger = NULL
};

static char *states_str[] __unused = {
        "STATE_ADJUST_BB",
        "STATE_IDLE",
        "STATE_LEFT",
        "STATE_FIRST_LEFT",
        "STATE_FIRST_RIGHT",
        "STATE_RIGHT"
};

static void
on_init(struct object *this)
{
        /* XXX: There is some serious issues with having a "large" stack
         * frame */
        static struct vdp1_cmdt_local_coord local;

        struct object_camera *camera;
        camera = (struct object_camera *)this;

        camera->transform.position.x = 0;
        camera->transform.position.y = 0;

        camera->private_data.m_median = SCREEN_WIDTH / 2;

        camera->private_data.m_buffer = object_player.private_data.m_width;

        camera->private_data.m_last_state = STATE_INVALID;
        camera->private_data.m_state = STATE_ADJUST_BB;

        struct bounding_box *lbb;
        lbb = &camera->private_data.m_lbb;

        lbb->point.a.x = 0;
        lbb->point.a.y = SCREEN_HEIGHT;
        lbb->point.b.y = SCREEN_HEIGHT;
        lbb->point.c.y = 0;
        lbb->point.d.x = 0;
        lbb->point.d.y = 0;

        struct bounding_box *rbb;
        rbb = &camera->private_data.m_rbb;
        rbb->point.a.y = SCREEN_HEIGHT;
        rbb->point.b.x = SCREEN_WIDTH;
        rbb->point.b.y = SCREEN_HEIGHT;
        rbb->point.c.x = SCREEN_WIDTH;
        rbb->point.c.y = 0;
        rbb->point.d.y = 0;

        struct vdp1_cmdt_line *debug_line;
        debug_line = &camera->private_data.m_debug_line;

        debug_line->cl_color = RGB888_TO_RGB555(255,   0, 255);
        debug_line->cl_mode.transparent_pixel = true;
        debug_line->cl_mode.end_code = true;
        debug_line->cl_vertex.a.x = camera->private_data.m_median;
        debug_line->cl_vertex.a.y = -SCREEN_HEIGHT;
        debug_line->cl_vertex.b.x = camera->private_data.m_median;
        debug_line->cl_vertex.b.y = 0;

        struct vdp1_cmdt_polygon *debug_polygon;
        debug_polygon = &camera->private_data.m_debug_polygon[0];

        debug_polygon[0].cp_color = RGB888_TO_RGB555(  0,   0, 196);
        debug_polygon[0].cp_mode.transparent_pixel = true;
        debug_polygon[0].cp_mode.end_code = true;
        debug_polygon[0].cp_mode.cc_mode = 3;

        debug_polygon[1].cp_color = RGB888_TO_RGB555(  0,   0, 196);
        debug_polygon[1].cp_mode.transparent_pixel = true;
        debug_polygon[1].cp_mode.end_code = true;
        debug_polygon[1].cp_mode.cc_mode = 3;

        struct cmd_group *cmd_group;
        cmd_group = &camera->private_data.m_cmd_group;

        cmd_group_init(cmd_group);

        cmd_group->priority = 0;

        local.lc_coord.x = 0;
        local.lc_coord.y = SCREEN_HEIGHT - 1;

        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_LOCAL_COORD, &local);
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_POLYGON, &debug_polygon[0]);
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_POLYGON, &debug_polygon[1]);
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_LINE, debug_line);
}

static void
on_update(struct object *this)
{
        struct object_camera *camera;
        camera = (struct object_camera *)this;

        struct object_player *player;
        player = (struct object_player *)&object_player;

        struct vdp1_cmdt_polygon *debug_polygon;
        debug_polygon = &camera->private_data.m_debug_polygon[0];

        struct bounding_box *lbb;
        lbb = &camera->private_data.m_lbb;
        struct bounding_box *rbb;
        rbb = &camera->private_data.m_rbb;

        cons_buffer(states_str[camera->private_data.m_state]);
        cons_buffer("\n");

        uint16_t old_position;
        old_position = camera->transform.position.x;
        uint16_t position __unused;
        position = old_position;

        int16_t left_x;
        int16_t right_x;

        int16_t amount;

        struct bounding_box player_bb;
        uint16_t player_width;
        player_width = player->private_data.m_width;
        uint16_t player_height;
        player_height = player->private_data.m_height;

        player_bb.point.a.x = 0;
        player_bb.point.a.y = player_height - 1;
        player_bb.point.b.x = player_width;
        player_bb.point.b.y = player_height - 1;
        player_bb.point.c.x = player_width;
        player_bb.point.c.y = 0;
        player_bb.point.d.x = 0;
        player_bb.point.d.y = 0;

        switch (camera->private_data.m_state) {
        case STATE_ADJUST_BB:
                left_x = object_player.transform.position.x - camera->private_data.m_buffer;
                lbb->point.b.x = left_x;
                lbb->point.c.x = left_x;

                right_x = object_player.transform.position.x + player_width + camera->private_data.m_buffer;
                rbb->point.a.x = right_x;
                rbb->point.d.x = right_x;

                camera->private_data.m_state = STATE_IDLE;
                break;
        case STATE_FIRST_LEFT:
        case STATE_LEFT:
        case STATE_IDLE:
                left_x = lbb->point.b.x;
                right_x = rbb->point.a.x;

                /* Right bounding box */
                amount = (object_player.transform.position.x + player_bb.point.b.x) - rbb->point.a.x;
                if (amount > 0) {
                        left_x += amount;
                        lbb->point.b.x = left_x;
                        lbb->point.c.x = left_x;

                        right_x += amount;
                        rbb->point.a.x = right_x;
                        rbb->point.d.x = right_x;

                        if ((camera->private_data.m_last_state != STATE_FIRST_RIGHT) &&
                            (camera->private_data.m_last_state != STATE_RIGHT) &&
                            ((rbb->point.a.x >= camera->private_data.m_median))) {
                                camera->private_data.m_last_state = camera->private_data.m_state;
                                camera->private_data.m_state = STATE_FIRST_RIGHT;
                        }

                        /* No need to continue to check the left bounding box */
                        break;
                }

                /* Left bounding box */
                amount = (object_player.transform.position.x + player_bb.point.a.x) - lbb->point.b.x;
                if (amount < 0) {
                        left_x += amount;
                        lbb->point.b.x = left_x;
                        lbb->point.c.x = left_x;

                        right_x += amount;
                        rbb->point.a.x = right_x;
                        rbb->point.d.x = right_x;

                        if ((camera->private_data.m_last_state != STATE_FIRST_LEFT) &&
                            (camera->private_data.m_last_state != STATE_LEFT)) {
                                camera->private_data.m_last_state = camera->private_data.m_state;
                                camera->private_data.m_state = STATE_FIRST_LEFT;
                        }
                }
                break;
        case STATE_FIRST_RIGHT:
                position = old_position + 1;

                amount = (object_player.transform.position.x + player_bb.point.b.x) - rbb->point.a.x;
                if (amount < 0) {
                        camera->private_data.m_state = STATE_IDLE;
                }
                break;
        case STATE_RIGHT:
                break;
        default:
                assert(false && "Invalid state");
        }

        camera->transform.position.x = position;

        debug_polygon[0].cp_vertex.a.x = lbb->point.b.x - 1;
        debug_polygon[0].cp_vertex.a.y = -lbb->point.b.y + 1;
        debug_polygon[0].cp_vertex.b.x = lbb->point.c.x - 1;
        debug_polygon[0].cp_vertex.b.y = lbb->point.c.y;
        debug_polygon[0].cp_vertex.c.x = lbb->point.d.x;
        debug_polygon[0].cp_vertex.c.y = lbb->point.d.y;
        debug_polygon[0].cp_vertex.d.x = lbb->point.a.x;
        debug_polygon[0].cp_vertex.d.y = -lbb->point.a.y + 1;

        debug_polygon[1].cp_vertex.a.x = rbb->point.b.x - 1;
        debug_polygon[1].cp_vertex.a.y = -rbb->point.b.y + 1;
        debug_polygon[1].cp_vertex.b.x = rbb->point.c.x - 1;
        debug_polygon[1].cp_vertex.b.y = rbb->point.c.y;
        debug_polygon[1].cp_vertex.c.x = rbb->point.d.x;
        debug_polygon[1].cp_vertex.c.y = rbb->point.d.y;
        debug_polygon[1].cp_vertex.d.x = rbb->point.a.x;
        debug_polygon[1].cp_vertex.d.y = -rbb->point.a.y + 1;

        cmd_groups_add(&camera->private_data.m_cmd_group);
}

static void
on_draw(struct object *this __unused)
{
}

/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include "physics.h"
#include "objects.h"

static void on_init(struct object *);
static void on_update(struct object *);
static void on_draw(struct object *);
static void on_collision(struct object *, struct object *,
    const struct collider_info *);
static void on_trigger(struct object *, const struct collider *);

static void _update_player(struct object *this);

static struct collider collider;
static struct rigid_body rigid_body;

struct object_player object_player = {
        .active = true,
        .id = OBJECT_PLAYER_ID,
        .rigid_body = &rigid_body,
        .colliders = {
                NULL
        },
        .on_init = on_init,
        .on_update = on_update,
        .on_draw = on_draw,
        .on_destroy = NULL,
        .on_collision = on_collision,
        .on_trigger = on_trigger
};

static void
on_init(struct object *this)
{
        /* XXX: There is some serious issues with having a "large" stack
         * frame */
        static struct vdp1_cmdt_local_coord local;

        struct object_player *player __unused;
        player = (struct object_player *)this;

        player->colliders[0] = &collider;

        player->private_data.m_direction = 1;

        player->private_data.m_width = 8;
        player->private_data.m_height = 16;

        player->private_data.m_state = PLAYER_STATE_IDLE;
        player->private_data.m_last_state = player->private_data.m_state;

        uint16_t width;
        width = player->private_data.m_width;
        uint16_t height;
        height = player->private_data.m_height;

        player->colliders[0]->object = (struct object *)player;
        player->colliders[0]->id = -1;
        player->colliders[0]->trigger = false;
        player->colliders[0]->fixed = false;
        player->colliders[0]->aabb.center.x = width / 2;
        player->colliders[0]->aabb.center.y = height / 2;
        player->colliders[0]->aabb.min.x = 0;
        player->colliders[0]->aabb.min.y = 0;
        player->colliders[0]->aabb.max.x = width - 1;
        player->colliders[0]->aabb.max.y = height - 1;
        player->colliders[0]->aabb.half.x = width / 2;
        player->colliders[0]->aabb.half.y = height / 2;

        player->private_data.m_polygon.cp_color = PLAYER_COLOR;
        player->private_data.m_polygon.cp_mode.transparent_pixel = true;
        player->private_data.m_polygon.cp_mode.end_code = true;

        physics_object_add((struct object *)player);

        struct cmd_group *cmd_group;
        cmd_group = &player->private_data.m_cmd_group;

        cmd_group_init(cmd_group);

        cmd_group->priority = 0;

        local.lc_coord.x = width / 2;
        local.lc_coord.y = SCREEN_HEIGHT - (height / 2);

        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_LOCAL_COORD, &local);
        cmd_group_add(cmd_group, CMD_GROUP_CMD_TYPE_POLYGON,
            &player->private_data.m_polygon);
}

static void
on_update(struct object *this)
{
        struct object_player *player __unused;
        player = (struct object_player *)this;

       int16_vector2_t old_position;
        old_position.x = object_player.transform.position.x;
        old_position.y = player->transform.position.y;

        int16_vector2_t new_position;
        memcpy(&new_position, &old_position, sizeof(int16_vector2_t));

        if (digital_pad.connected == 1) {
                if (digital_pad.pressed.button.left) {
                        player->private_data.m_direction = -1;
                        player->private_data.m_state = PLAYER_STATE_RUN;

                        new_position.x = player->transform.position.x +
                            player->private_data.m_direction;
                } else if (digital_pad.pressed.button.right) {
                        player->private_data.m_direction = 1;
                        player->private_data.m_state = PLAYER_STATE_RUN;

                        new_position.x = player->transform.position.x +
                            player->private_data.m_direction;
                } else {
                        player->private_data.m_state = PLAYER_STATE_IDLE;
                }

                if (digital_pad.pressed.button.up) {
                        player->private_data.m_state = PLAYER_STATE_RUN;

                        new_position.y = player->transform.position.y + 1;
                } else if (digital_pad.pressed.button.down) {
                        player->private_data.m_state = PLAYER_STATE_RUN;

                        new_position.y = player->transform.position.y - 1;
                }
        }

        player->transform.position.x = new_position.x;
        player->transform.position.y = new_position.y;

        switch  (player->private_data.m_state) {
        case PLAYER_STATE_IDLE:
                player->private_data.m_polygon.cp_color = PLAYER_COLOR;
                break;
        case PLAYER_STATE_RUN:
                break;
        case PLAYER_STATE_JUMP:
                player->private_data.m_polygon.cp_color =
                    0x8000 | RGB888_TO_RGB555(0, 255, 0);
                break;
        default:
                assert(false && "Invalid state");
        }

        _update_player(this);

        cmd_groups_add(&player->private_data.m_cmd_group);
}

static void
on_draw(struct object *this)
{
        struct object_player *player __unused;
        player = (struct object_player *)this;
}

static void
on_collision(struct object *this, struct object *other,
    const struct collider_info * info)
{
        struct object_player *player __unused;
        player = (struct object_player *)this;

        (void)sprintf(text, "object->id=%i, other->id=%i, o=%i, d=%i,%i, P=%i,%i\n",
            (int)this->id,
            (int)other->id,
            (int)info->overlap,
            (int)info->direction.x,
            (int)info->direction.y,
            (int)player->transform.position.x,
            (int)player->transform.position.y);
        cons_buffer(&cons, text);

        _update_player(this);
}

static void
on_trigger(struct object *this, const struct collider * collider __unused)
{
        struct object_player *player __unused;
        player = (struct object_player *)this;
}

static void
_update_player(struct object *this)
{
        struct object_player *player;
        player = (struct object_player *)this;

        struct vdp1_cmdt_polygon *polygon;
        polygon = &player->private_data.m_polygon;

        uint16_t width;
        width = player->private_data.m_width;
        uint16_t height;
        height = player->private_data.m_height;

        int16_t x;
        x = player->transform.position.x;
        int16_t y;
        y = player->transform.position.y;

        (void)sprintf(text, "P=%i,%i\n", (int)x, (int)y);
        cons_buffer(&cons, text);

        polygon->cp_vertex.a.x = (width / 2) + x - 1;
        polygon->cp_vertex.a.y = -(height / 2) - y;
        polygon->cp_vertex.b.x = (width / 2) + x - 1;
        polygon->cp_vertex.b.y = (height / 2) - y - 1;
        polygon->cp_vertex.c.x = -(width / 2) + x;
        polygon->cp_vertex.c.y = (height / 2) - y - 1;
        polygon->cp_vertex.d.x = -(width / 2) + x;
        polygon->cp_vertex.d.y = -(height / 2) - y;
}

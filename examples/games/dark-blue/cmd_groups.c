/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_groups.h"
#include "common.h"
#include "globals.h"
#include "objects.h"

#define CMD_GROUP_POOL_MAX 3

static struct cmd_group *cmd_group_pool[CMD_GROUP_POOL_MAX];
static uint32_t pool_idx = 0;

void
cmd_groups_init(void)
{
        pool_idx = 0;

        memset(&cmd_group_pool[0], 0x00, sizeof(cmd_group_pool));
}

void
cmd_groups_add(struct cmd_group *cmd_group)
{
        assert(pool_idx < CMD_GROUP_POOL_MAX);

        assert(cmd_group != NULL);

        cmd_group_pool[pool_idx] = cmd_group;
        pool_idx++;
}

void
cmd_groups_update(void)
{
        vdp1_cmdt_list_begin(0); {
                struct vdp1_cmdt_system_clip_coord system_clip;
                system_clip.scc_coord.x = SCREEN_WIDTH - 1;
                system_clip.scc_coord.y = SCREEN_HEIGHT - 1;

                static struct vdp1_cmdt_user_clip_coord user_clip;
                user_clip.ucc_coords[0].x = 0;
                user_clip.ucc_coords[0].y = 0;
                user_clip.ucc_coords[1].x = SCREEN_WIDTH - 1;
                user_clip.ucc_coords[1].y = SCREEN_HEIGHT - 1;

                struct vdp1_cmdt_local_coord local;
                local.lc_coord.x = 0;
                local.lc_coord.y = SCREEN_HEIGHT - 1;

                vdp1_cmdt_system_clip_coord_set(&system_clip);
                vdp1_cmdt_user_clip_coord_set(&user_clip);
                vdp1_cmdt_local_coord_set(&local);

                uint32_t idx;
                for (idx = 0; idx < pool_idx; idx++) {
                        struct cmd_group *cmd_group;
                        cmd_group = cmd_group_pool[idx];

                        uint32_t cmd_idx;
                        for (cmd_idx = 0; cmd_idx < cmd_group->cmds_cnt;
                             cmd_idx++) {
                                struct cmd *cmd;
                                cmd = &cmd_group->cmds[cmd_idx];

                                switch (cmd->type) {
                                case CMD_GROUP_CMD_TYPE_LOCAL_COORD:
                                        vdp1_cmdt_local_coord_set(cmd->data);
                                        break;
                                case CMD_GROUP_CMD_TYPE_SYSTEM_CLIP_COORD:
                                        vdp1_cmdt_system_clip_coord_set(
                                                cmd->data);
                                        break;
                                case CMD_GROUP_CMD_TYPE_USER_CLIP_COORD:
                                        vdp1_cmdt_user_clip_coord_set(
                                                cmd->data);
                                        break;
                                case CMD_GROUP_CMD_TYPE_POLYGON:
                                        vdp1_cmdt_polygon_draw(cmd->data);
                                        break;
                                case CMD_GROUP_CMD_TYPE_LINE:
                                        vdp1_cmdt_line_draw(cmd->data);
                                        break;
                                case CMD_GROUP_CMD_TYPE_NORMAL_SPRITE:
                                        vdp1_cmdt_sprite_draw(cmd->data);
                                        break;
                                default:
                                        assert(false && "Invalid command group type specified");
                                }
                        }
                }
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);

        pool_idx = 0;
}

void
cmd_group_init(struct cmd_group *cmd_group)
{
        assert(cmd_group != NULL);

        cmd_group->priority = -1;

        memset(&cmd_group->cmds[0], 0x00, sizeof(cmd_group->cmds));
        cmd_group->cmds_cnt = 0;
}

void
cmd_group_add(struct cmd_group *cmd_group, uint32_t type, void *data)
{
        assert(cmd_group != NULL);
        assert(cmd_group->priority >= 0);

        uint32_t cmd_idx;
        cmd_idx = cmd_group->cmds_cnt;

        struct cmd *cmd;
        cmd = &cmd_group->cmds[cmd_idx];

        cmd->type = type;
        cmd->data = data;

        cmd_group->cmds_cnt++;
}

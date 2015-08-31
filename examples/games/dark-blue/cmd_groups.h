/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef CMD_GROUPS_H
#define CMD_GROUPS_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

struct cmd_group {
#define CMD_GROUPS_MAX   32

        int32_t priority;

        struct cmd {
#define CMD_GROUP_CMD_TYPE_LOCAL_COORD              1
#define CMD_GROUP_CMD_TYPE_SYSTEM_CLIP_COORD        2
#define CMD_GROUP_CMD_TYPE_USER_CLIP_COORD          3
#define CMD_GROUP_CMD_TYPE_POLYGON                  4
#define CMD_GROUP_CMD_TYPE_LINE                     5
                uint32_t type;
                void *data;
        } cmds[CMD_GROUPS_MAX];
        uint32_t cmds_cnt;
};

void cmd_groups_init(void);
void cmd_groups_update(void);
void cmd_groups_add(struct cmd_group *);

void cmd_group_init(struct cmd_group *);
void cmd_group_add(struct cmd_group *, uint32_t, void *);

#endif /* !CMD_GROUPS_H */

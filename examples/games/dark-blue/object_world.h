/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef OBJECT_WORLD_H
#define OBJECT_WORLD_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include "bounding_box.h"
#include "common.h"
#include "globals.h"
#include "object.h"
#include "state.h"

struct world_column {
#define WORLD_BLOCK_WIDTH      16
#define WORLD_BLOCK_HEIGHT     16

#define WORLD_ROWS      (SCREEN_HEIGHT / WORLD_BLOCK_HEIGHT)
#define WORLD_COLUMNS   (SCREEN_WIDTH / WORLD_BLOCK_WIDTH)

        struct {
#define WORLD_COLUMN_BLOCK_NONE         0
#define WORLD_COLUMN_BLOCK_SAFE         1
#define WORLD_COLUMN_BLOCK_UNSAFE       2
#define WORLD_COLUMN_BLOCK_NO_COLLIDER  3
#define WORLD_COLUMN_BLOCK_COLLIDER     4
                uint16_t block;
        } row[WORLD_ROWS];
};

struct object_world {
/* |...|\n */
#define WORLD_MAP_FORMAT_COLUMN_SIZE    WORLD_ROWS

#define WORLD_MAP_COLUMN_BLOCK_SAFE_COLOR       RGB888_TO_RGB555(255, 255, 255)
#define WORLD_MAP_COLUMN_BLOCK_UNSAFE_COLOR     RGB888_TO_RGB555(255,  99,  99)

        OBJECT_DECLARATIONS

        /* Public data */
        struct {
                void (*m_start)(struct object *);
        } functions;

        struct {
        } data;

        /* Private data */
        struct {
                void (*m_update_map)(struct object *, uint32_t);
                void (*m_update_colliders)(struct object *);
        } private_functions;

        struct {
                void *m_map_fh;
                struct world_column m_columns[WORLD_COLUMNS + 1];
                uint32_t m_column_count;
                uint32_t m_row_count;
                uint16_t m_scroll_x;
                uint16_t m_last_scroll_x;
                uint16_t m_move;
                uint16_t m_column;
                struct scrn_cell_format m_nbg1_format;
                struct scrn_cell_format m_nbg2_format;
        } private_data;
};

bool object_world_collide(struct bounding_box *, int16_vector2_t *);

extern struct object_world object_world;

#endif /* !OBJECT_WORLD_H */

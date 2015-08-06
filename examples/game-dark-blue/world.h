/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef WORLD_H
#define WORLD_H

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

        struct {
        } data;

        struct {
                void *m_map_fh;
                struct world_column m_columns[WORLD_COLUMNS + 1];
                uint32_t m_column_count;
                uint32_t m_row_count;

                struct scrn_cell_format m_nbg1_format;
                uint16_t *m_nbg1_planes[4];
                uint8_t *m_nbg1_character;
                uint16_t *m_nbg1_color_palette;

                struct scrn_cell_format m_nbg2_format;
                uint16_t *m_nbg2_planes[4];
                uint8_t *m_nbg2_character;
                uint16_t *m_nbg2_color_palette;
        } private_data;
};

bool object_world_collide(struct bounding_box *, int16_vector2_t *);

extern struct object_world object_world;

#endif /* !WORLD_H */

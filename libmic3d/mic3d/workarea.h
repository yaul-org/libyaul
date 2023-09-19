/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_WORKAREA_H_
#define _MIC3D_WORKAREA_H_

#include <mic3d/config.h>
#include <mic3d/sizes.h>

#define POOL_SORT_LISTS_COUNT      SORT_DEPTH
#define POOL_SORT_SINGLES_COUNT    CMDT_COUNT
#define POOL_CMDTS_COUNT           CMDT_COUNT
#define POOL_DEPTH_VALUES_COUNT    POINTS_COUNT
#define POOL_Z_VALUES_COUNT        POINTS_COUNT
#define POOL_SCREEN_POINTS_COUNT   POINTS_COUNT
#define POOL_RENDER_MATRICES_COUNT 3
#define POOL_LIGHT_MATRICES_COUNT  5
#define POOL_COLORS_COUNT          POINTS_COUNT

#define WORKAREA_SORT_LISTS_BYTE_SIZE      (STRUCT_SORT_LISTS_BYTE_SIZE * POOL_SORT_LISTS_COUNT)
#define WORKAREA_SORT_SINGLES_BYTE_SIZE    (STRUCT_SORT_SINGLES_BYTE_SIZE * POOL_SORT_SINGLES_COUNT)
#define WORKAREA_CMDTS_BYTE_SIZE           (STRUCT_CMDTS_BYTE_SIZE * POOL_CMDTS_COUNT)
#define WORKAREA_DEPTH_VALUES_BYTE_SIZE    (STRUCT_DEPTH_VALUES_BYTE_SIZE * POOL_DEPTH_VALUES_COUNT)
#define WORKAREA_Z_VALUES_BYTE_SIZE        (STRUCT_Z_VALUES_BYTE_SIZE * POOL_Z_VALUES_COUNT)
#define WORKAREA_SCREEN_POINTS_BYTE_SIZE   (STRUCT_SCREEN_POINTS_BYTE_SIZE * POOL_SCREEN_POINTS_COUNT)
#define WORKAREA_RENDER_MATRICES_BYTE_SIZE (STRUCT_RENDER_MATRICES_BYTE_SIZE * POOL_RENDER_MATRICES_COUNT)
#define WORKAREA_LIGHT_MATRICES_BYTE_SIZE  (STRUCT_LIGHT_MATRICES_BYTE_SIZE * POOL_LIGHT_MATRICES_COUNT)
#define WORKAREA_COLORS_BYTE_SIZE          (STRUCT_COLORS_BYTE_SIZE * POOL_COLORS_COUNT)
#define WORKAREA_WORK_BYTE_SIZE            (STRUCT_WORK_BYTE_SIZE)

#define WORKAREA_SORT_LISTS_ALIGNMENT      16
#define WORKAREA_SORT_SINGLES_ALIGNMENT    16
#define WORKAREA_CMDTS_ALIGNMENT           16
#define WORKAREA_DEPTH_VALUES_ALIGNMENT    16
#define WORKAREA_Z_VALUES_ALIGNMENT        16
#define WORKAREA_SCREEN_POINTS_ALIGNMENT   16
#define WORKAREA_RENDER_MATRICES_ALIGNMENT 16
#define WORKAREA_LIGHT_MATRICES_ALIGNMENT  16
#define WORKAREA_COLORS_ALIGNMENT          16
#define WORKAREA_WORK_ALIGNMENT            16

#define __WORKAREA_DECLARE_STRUCT(lname_, uname_)                              \
    typedef struct mic3d_workarea_##lname_ {                                   \
        uint8_t pool[WORKAREA_##uname_##_BYTE_SIZE];                           \
    } __aligned(WORKAREA_##uname_##_ALIGNMENT) mic3d_workarea_##lname_##_t

__WORKAREA_DECLARE_STRUCT(sort_lists, SORT_LISTS);
__WORKAREA_DECLARE_STRUCT(sort_singles, SORT_SINGLES);
__WORKAREA_DECLARE_STRUCT(cmdts, CMDTS);
__WORKAREA_DECLARE_STRUCT(depth_values, DEPTH_VALUES);
__WORKAREA_DECLARE_STRUCT(z_values, Z_VALUES);
__WORKAREA_DECLARE_STRUCT(screen_points, SCREEN_POINTS);
__WORKAREA_DECLARE_STRUCT(render_matrices, RENDER_MATRICES);
__WORKAREA_DECLARE_STRUCT(light_matrices, LIGHT_MATRICES);
__WORKAREA_DECLARE_STRUCT(colors, COLORS);
__WORKAREA_DECLARE_STRUCT(work, WORK);

#undef __WORKAREA_DECLARE_STRUCT

typedef struct mic3d_workarea {
    mic3d_workarea_depth_values_t *depth_values;
    mic3d_workarea_z_values_t *z_values;
    mic3d_workarea_screen_points_t *screen_points;
    mic3d_workarea_sort_lists_t *sort_lists;
    mic3d_workarea_sort_singles_t *sort_singles;
    mic3d_workarea_cmdts_t *cmdts;
    mic3d_workarea_render_matrices_t *render_matrices;
    mic3d_workarea_light_matrices_t *light_matrices;
    mic3d_workarea_colors_t *colors;
    mic3d_workarea_work_t *work;
} mic3d_workarea_t;

#endif /* _MIC3D_WORKAREA_H_ */

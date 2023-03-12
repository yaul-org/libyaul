/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "internal.h"

#define POOL_POINTS_COUNT        POINTS_COUNT
#define POOL_POLYGONS_COUNT      POLYGON_COUNT
#define POOL_SORT_LIST_COUNT     SORT_DEPTH
#define POOL_SORT_SINGLES_COUNT  CMDT_COUNT
#define POOL_CMDTS_COUNT         CMDT_COUNT
#define POOL_DEPTH_VALUES_COUNT  POINTS_COUNT
#define POOL_Z_VALUES_COUNT      POINTS_COUNT
#define POOL_SCREEN_POINTS_COUNT POINTS_COUNT
#define POOL_MATRIX_STACK_COUNT  MATRIX_STACK_COUNT
#define POOL_COLORS_COUNT        POINTS_COUNT

fix16_t __pool_depth_values[POOL_DEPTH_VALUES_COUNT] __aligned(16);
fix16_t __pool_z_values[POOL_Z_VALUES_COUNT] __aligned(16);
int16_vec2_t __pool_screen_points[POOL_SCREEN_POINTS_COUNT] __aligned(16);

sort_list_t __pool_sort_lists[POOL_SORT_LIST_COUNT] __aligned(16);
sort_single_t __pool_sort_singles[POOL_SORT_SINGLES_COUNT] __aligned(16);

vdp1_cmdt_t __pool_cmdts[POOL_CMDTS_COUNT] __aligned(16);

fix16_mat43_t __pool_matrices[POOL_MATRIX_STACK_COUNT] __aligned(16);

rgb1555_t __pool_colors[POOL_COLORS_COUNT] __aligned(16);

render_transform_t __render_transform __aligned(16);

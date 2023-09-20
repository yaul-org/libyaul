/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_INTERNAL_H_
#define _MIC3D_INTERNAL_H_

#include "mic3d/sizes.h"

#include "render.h"
#include "sort.h"
#include "tlist.h"
#include "camera.h"
#include "light.h"
#include "state.h"
#include "perf.h"
#include "gst.h"

static_assert(sizeof(sort_list_t) == STRUCT_MIC3D_SORT_LISTS_BYTE_SIZE);
static_assert(sizeof(sort_single_t) == STRUCT_MIC3D_SORT_SINGLES_BYTE_SIZE);
static_assert(sizeof(vdp1_cmdt_t) == STRUCT_MIC3D_CMDTS_BYTE_SIZE);
static_assert(sizeof(fix16_t) == STRUCT_MIC3D_DEPTH_VALUES_BYTE_SIZE);
static_assert(sizeof(fix16_t) == STRUCT_MIC3D_Z_VALUES_BYTE_SIZE);
static_assert(sizeof(int16_vec2_t) == STRUCT_MIC3D_SCREEN_POINTS_BYTE_SIZE);
static_assert(sizeof(fix16_mat43_t) == STRUCT_MIC3D_RENDER_MATRICES_BYTE_SIZE);
static_assert(sizeof(fix16_mat33_t) == STRUCT_MIC3D_LIGHT_MATRICES_BYTE_SIZE);
static_assert(sizeof(rgb1555_t) == STRUCT_MIC3D_COLORS_BYTE_SIZE);
static_assert(sizeof(render_transform_t) == STRUCT_MIC3D_WORK_BYTE_SIZE);

#endif /* _MIC3D_INTERNAL_H_ */

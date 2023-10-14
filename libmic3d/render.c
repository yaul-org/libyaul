/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include <cpu/cache.h>
#include <cpu/divu.h>
#include <cpu/intc.h>
#include <cpu/registers.h>

#include <gamemath/fix16.h>

#include "gamemath/fix16/fix16_mat33.h"
#include "gamemath/fix16/fix16_mat43.h"
#include "internal.h"
#include "vdp1/cmdt.h"

/* TODO: Remove constant */
#define SCREEN_RATIO (SCREEN_WIDTH / (float)SCREEN_HEIGHT)

#define SCREEN_CLIP_LEFT   ( -SCREEN_WIDTH / 2)
#define SCREEN_CLIP_RIGHT  (  SCREEN_WIDTH / 2)
#define SCREEN_CLIP_TOP    (-SCREEN_HEIGHT / 2)
#define SCREEN_CLIP_BOTTOM ( SCREEN_HEIGHT / 2)

#define NEAR_LEVEL_MIN 1U
#define NEAR_LEVEL_MAX 8U
#define FAR_LEVEL_MIN  1U
#define FAR_LEVEL_MAX  3U

#define ORDER_SYSTEM_CLIP_COORDS_INDEX 0
#define ORDER_LOCAL_COORDS_INDEX       1
#define ORDER_CLEAR_POLYGON_INDEX      2
#define ORDER_SUBR_INDEX               2
#define ORDER_DRAW_END_INDEX           3
#define ORDER_INDEX                    4

#define MATRIX_INDEX_CAMERA 0
#define MATRIX_INDEX_VIEW   1

static void _render_reset(void);

static void _vdp1_init(void);

static void _orthographic_transform(void);
static void _perspective_transform(void);

static inline int32_t _depth_normalize(fix16_t z);
static int16_t _depth_min_calculate(const int16_t *z_values);
static int16_t _depth_max_calculate(const int16_t *z_values);
static int16_t _depth_center_calculate(const int16_t *z_values);

static bool _pipeline_backface_cull_test(void);

static void _clip_flags_calculate(const int16_vec2_t *screen_point,
    clip_flags_t *clip_flags, clip_flags_t *and_flags, clip_flags_t *or_flags);
static void _clip_flags_lrtb_calculate(const int16_vec2_t screen_point,
    clip_flags_t *clip_flag);

static void _screen_points_swap(int16_vec2_t *screen_points, uint32_t i,
    uint32_t j);

static void _pipeline_polygon_orient(void);

static uint32_t _cmdts_count_get(void);
static vdp1_cmdt_t *_cmdts_alloc(void);
static void _cmdts_reset(void);
static void _cmdt_process(vdp1_cmdt_t *cmdt);
static void _cmdts_insert(vdp1_cmdt_t *cmdt);

static void _cpu_divu_ovfi_handler(void);

static perf_counter_t _transform_pc __unused;
static perf_counter_t _sort_pc __unused;

void
__render_init(void)
{
    workarea_mic3d_t * const workarea = __state.workarea;
    render_t * const render = __state.render;

    render->z_values_pool = (void *)workarea->z_values;
    render->screen_points_pool = (void *)workarea->screen_points;
    render->depth_values_pool = (void *)workarea->depth_values;
    render->cmdts_pool = (void *)workarea->cmdts;

    render->mesh = NULL;
    render->world_matrix = NULL;

    fix16_mat43_t * const render_matrices = (void *)workarea->render_matrices;

    render->camera_matrix = &render_matrices[MATRIX_INDEX_CAMERA];
    render->view_matrix = &render_matrices[MATRIX_INDEX_VIEW];
    render->pipeline = (void *)workarea->work;

    render->render_flags = RENDER_FLAGS_NONE;

    fix16_mat43_identity(render->view_matrix);
    fix16_mat43_identity(render->camera_matrix);

    cpu_divu_ovfi_set(_cpu_divu_ovfi_handler);

    render_perspective_set(DEG2ANGLE(90.0f));
    render_orthographic_set(FIX16(10.0f));
    render_near_level_set(7);
    render_far_level_set(0);
    camera_type_set(CAMERA_TYPE_PERSPECTIVE);

    _render_reset();

    __perf_counter_init(&_transform_pc);
    __perf_counter_init(&_sort_pc);

    _vdp1_init();
}

void
render_enable(render_flags_t flags)
{
    render_t * const render = __state.render;

    render->render_flags |= flags;
}

void
render_disable(render_flags_t flags)
{
    render_t * const render = __state.render;

    render->render_flags &= ~flags;
}

void
render_flags_set(render_flags_t flags)
{
    render_t * const render = __state.render;

    render->render_flags = flags;
}

void
render_perspective_set(angle_t fov_angle)
{
    render_t * const render = __state.render;

    render->view_distance = math3d_view_distance_calc(SCREEN_WIDTH, fov_angle);
}

void
render_orthographic_set(fix16_t scale)
{
    render_t * const render = __state.render;

    render->ortho_size = fix16_div(FIX16(SCREEN_WIDTH * SCREEN_RATIO), scale);
}

void
render_near_level_set(uint32_t level)
{
    render_t * const render = __state.render;

    const uint32_t clamped_level = clamp(level + 1, NEAR_LEVEL_MIN,
        NEAR_LEVEL_MAX);

    uint32_t near_value;
    near_value = render->view_distance;

    for (int32_t i = clamped_level; i > 0; i--) {
        /* Prevent GCC from doing an arithmetic shift (results in a
         * function call) */
        near_value >>= 1;
    }

    render->near = near_value;
}

void
render_far_level_set(uint32_t level)
{
    /*
     * 0: Origin
     * V: View distance from: 0.5*(screen_width-1)*tan(0.5*hfov)
     *
     * Near values via levels:
     *  a: V/(1<<4)
     *  b: V/(1<<3)
     *  c: V/(1<<2)
     *  d: V/(1<<1)
     *
     * Far values via levels:
     *  f: V*(1<<1)
     *  g: V*(1<<2)
     *  h: V*(1<<3)
     *
     *  +------------------------------------------------------------------------> -Z
     *  ||| |  |   |     |                    |            |                  |
     *  ||| |  |   |     |                    |            |                  |
     *  0 a b  c   d     V                    f            g                  h
     */

    render_t * const render = __state.render;

    const uint32_t clamped_level =
        clamp(level, FAR_LEVEL_MIN - 1, FAR_LEVEL_MAX);

    uint32_t far_value;
    far_value = render->view_distance;

    for (int32_t i = clamped_level; i > 0; i--) {
        /* Prevent GCC from doing an arithmetic shift (results in a function
         * call) */
        far_value <<= 1;
    }

    render->far = far_value;
}

void
render_sort_depth_set(sort_list_t *sort_list_buffer, uint16_t count)
{
    assert(sort_list_buffer != NULL);
    assert((count & (count - 1)) == 0);

    render_t * const render = __state.render;
    sort_t * const sort = __state.sort;

    sort->sort_lists_pool = sort_list_buffer;

    /* To normalize Z [0,1]:            a=-1/(f-n)    The - is to negate the Z-axis
     *                                  b=a*n
     *                       normalized_z=(a*z)+b
     *
     * We also at the same time need to normalize to the sort scale */
    render->sort_scale = fix16_int32_from(count - 1);
    render->depth_scale = fix16_div(FIX16(-1.0f), render->far - render->near);
    render->depth_offset = fix16_mul(render->depth_scale, render->near);
}

void
render_point_xform(const fix16_mat43_t *world_matrix, const fix16_vec3_t *point,
    xform_t *xform)
{
    assert(world_matrix != NULL);
    assert(point != NULL);
    assert(xform != NULL);

    render_t * const render = __state.render;

    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    fix16_mat43_t view_matrix;
    fix16_mat43_mul(camera_matrix, world_matrix, &view_matrix);

    if (render->camera_type == CAMERA_TYPE_PERSPECTIVE) {
        math3d_point_perspective_xform(&view_matrix, render->view_distance, point, xform);
    } else {
        math3d_point_orthographic_xform(&view_matrix, render->ortho_size,  point, xform);
    }
}

void
render_points_xform(const fix16_mat43_t *world_matrix, const fix16_vec3_t *points,
    xform_t *xforms, uint32_t count)
{
    assert(world_matrix != NULL);
    assert(points != NULL);
    assert(xforms != NULL);

    render_t * const render = __state.render;

    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    fix16_mat43_t view_matrix;
    fix16_mat43_mul(camera_matrix, world_matrix, &view_matrix);

    if (render->camera_type == CAMERA_TYPE_PERSPECTIVE) {
        for (uint32_t i = 0; i < count; i++) {
            math3d_point_perspective_xform(&view_matrix, render->view_distance, &points[i], &xforms[i]);
        }
    } else {
        for (uint32_t i = 0; i < count; i++) {
            math3d_point_orthographic_xform(&view_matrix, render->ortho_size, points, &xforms[i]);
        }
    }
}

void
render_start(void)
{
}

void
render_mesh_xform(const mesh_t *mesh, const fix16_mat43_t *world_matrix)
{
    assert(mesh != NULL);
    assert(world_matrix != NULL);

    render_t * const render = __state.render;
    pipeline_t * const pipeline = render->pipeline;

    light_polygon_processor_t light_polygon_processor;

    render->mesh = mesh;
    render->world_matrix = world_matrix;

    __perf_counter_start(&_transform_pc);

    if (render->camera_type == CAMERA_TYPE_PERSPECTIVE) {
        _perspective_transform();
    } else if (render->camera_type == CAMERA_TYPE_ORTHOGRAPHIC) {
        _orthographic_transform();
    }

    __light_transform(&light_polygon_processor);

    int16_t * const z_values = render->z_values_pool;
    int16_vec2_t * const screen_points = render->screen_points_pool;
    const polygon_t * const polygons = render->mesh->polygons;

    for (uint32_t i = 0; i < render->mesh->polygons_count; i++) {
        pipeline->polygon = polygons[i];

        pipeline->screen_points[0] =
            screen_points[pipeline->polygon.indices.p0];
        pipeline->screen_points[1] =
            screen_points[pipeline->polygon.indices.p1];
        pipeline->screen_points[2] =
            screen_points[pipeline->polygon.indices.p2];
        pipeline->screen_points[3] =
            screen_points[pipeline->polygon.indices.p3];

        if (pipeline->polygon.flags.plane_type != PLANE_TYPE_DOUBLE) {
            if ((_pipeline_backface_cull_test())) {
                continue;
            }
        }

        pipeline->z_values[0] = z_values[pipeline->polygon.indices.p0];
        pipeline->z_values[1] = z_values[pipeline->polygon.indices.p1];
        pipeline->z_values[2] = z_values[pipeline->polygon.indices.p2];
        pipeline->z_values[3] = z_values[pipeline->polygon.indices.p3];

        const int16_t min_depth_z = _depth_min_calculate(pipeline->z_values);

        /* Cull polygons intersecting with the near plane */
        if (min_depth_z < 0) {
            continue;
        }

        const int16_t max_depth_z = _depth_max_calculate(pipeline->z_values);

        if (max_depth_z > render->sort_scale) {
            continue;
        }

        _clip_flags_calculate(pipeline->screen_points, pipeline->clip_flags,
            &pipeline->and_flags, &pipeline->or_flags);

        /* Cull if the polygon is entirely off screen */
        if (pipeline->and_flags != CLIP_FLAGS_NONE) {
            continue;
        }

        int32_t depth_z;

        switch (pipeline->polygon.flags.sort_type) {
        default:
        case SORT_TYPE_CENTER:
            depth_z = _depth_center_calculate(pipeline->z_values);
            break;
        case SORT_TYPE_MIN:
            depth_z = min_depth_z;
            break;
        case SORT_TYPE_MAX:
            depth_z = max_depth_z;
            break;
        }

        __sort_insert(depth_z);

        pipeline->attribute = render->mesh->attributes[i];

        if (pipeline->or_flags == CLIP_FLAGS_NONE) {
            /* If no clip flags are set, disable pre-clipping. This
             * should help with performance */
            pipeline->attribute.draw_mode.pre_clipping_disable = true;
        } else {
            _pipeline_polygon_orient();
        }

        light_polygon_processor();

        vdp1_cmdt_t * const cmdt = _cmdts_alloc();

        _cmdt_process(cmdt);
    }
}

void
render_cmdt_nocheck_insert(const vdp1_cmdt_t *cmdt, fix16_t depth_z)
{
    assert(cmdt != NULL);

    const vdp1_cmdt_command_t command = vdp1_cmdt_command_get(cmdt);

    if (command >= VDP1_CMDT_USER_CLIP_COORD) {
        return;
    }

    render_t * const render = __state.render;

    const int32_t normalized_z = _depth_normalize(depth_z);

    if (normalized_z < 0) {
        return;
    }

    if (normalized_z > render->sort_scale) {
        return;
    }

    vdp1_cmdt_t * const to_cmdt = _cmdts_alloc();

    /* Copy command table. Compiler will probably stick a memcpy() here */
    *to_cmdt = *cmdt;

    _cmdts_insert(to_cmdt);
    __sort_insert(normalized_z);
}

void
render_cmdt_insert(const vdp1_cmdt_t *cmdt, fix16_t depth_z)
{
    assert(cmdt != NULL);

    const vdp1_cmdt_command_t command = vdp1_cmdt_command_get(cmdt);

    if (command >= VDP1_CMDT_USER_CLIP_COORD) {
        return;
    }

    render_t * const render = __state.render;

    const int32_t normalized_z = _depth_normalize(depth_z);

    if (normalized_z < 0) {
        return;
    }

    if (normalized_z > render->sort_scale) {
        return;
    }

    /* Calculate clip flags to either cull or orient vertices */
    clip_flags_t clip_flags[4];
    clip_flags_t and_flags;
    clip_flags_t or_flags;

    _clip_flags_calculate(cmdt->cmd_vertices, clip_flags, &and_flags, &or_flags);

    /* Cull if the polygon is entirely off screen */
    if (and_flags != CLIP_FLAGS_NONE) {
        return;
    }

    vdp1_cmdt_t * const to_cmdt = _cmdts_alloc();

    /* Copy command table. Compiler will probably stick a memcpy() here */
    *to_cmdt = *cmdt;

    _cmdts_insert(to_cmdt);

    if (or_flags == CLIP_FLAGS_NONE) {
        /* If no clip flags are set, disable pre-clipping. This should help with
         * performance */
        to_cmdt->cmd_draw_mode.pre_clipping_disable = true;
    } else {
        if ((clip_flags[0] & CLIP_FLAGS_LR) != CLIP_FLAGS_NONE) {
            /* B-|-A
             * | | |
             * C-|-D
             *   |
             *   | Outside
             *
             * Swap A & B
             * Swap D & C */

            _screen_points_swap(to_cmdt->cmd_vertices, 0, 1);
            _screen_points_swap(to_cmdt->cmd_vertices, 2, 3);

            to_cmdt->cmd_ctrl ^= VDP1_CMDT_CHAR_FLIP_H;
        }

        if ((clip_flags[0] & CLIP_FLAGS_TB) != CLIP_FLAGS_NONE) {
            /*   B---A Outside
             * --|---|--
             *   C---D
             *
             *   Swap A & D
             *   Swap B & C */

            _screen_points_swap(to_cmdt->cmd_vertices, 0, 3);
            _screen_points_swap(to_cmdt->cmd_vertices, 1, 2);

            to_cmdt->cmd_ctrl ^= VDP1_CMDT_CHAR_FLIP_V;
        }
    }

    __sort_insert(normalized_z);
}

void
render_cmdts_reserve(uint32_t cmdt_count __unused)
{
}

void
render_cmdts_relinquish(void)
{
}

void
render_end(void)
{
    render_t * const render = __state.render;

    vdp1_cmdt_t * const subr_cmdt = (vdp1_cmdt_t *)
        VDP1_CMD_TABLE(ORDER_SUBR_INDEX, 0);

    const uint32_t cmdt_count = _cmdts_count_get();

    if (cmdt_count == 0) {
        vdp1_cmdt_link_type_set(subr_cmdt, VDP1_CMDT_LINK_TYPE_JUMP_NEXT);

        /* Force a render to invoke a put callback */
        vdp1_sync_force_put();

        return;
    }

    render->sort_cmdt = subr_cmdt;
    render->sort_link = ORDER_INDEX;

    /* Set as a subroutine call. If RENDER_FLAGS_NO_CLEAR is set, skip the clear
     * polygon (ORDER_SUBR_INDEX), but start the call */
    if (RENDER_FLAG_TEST(NO_CLEAR)) {
        vdp1_cmdt_link_type_set(subr_cmdt, VDP1_CMDT_LINK_TYPE_SKIP_CALL);

        vdp1_sync_mode_set(VDP1_SYNC_MODE_CHANGE_ONLY);
    } else {
        vdp1_cmdt_link_type_set(subr_cmdt, VDP1_CMDT_LINK_TYPE_JUMP_CALL);

        vdp1_sync_mode_set(VDP1_SYNC_MODE_ERASE_CHANGE);
    }

    __perf_counter_start(&_sort_pc);
    __sort_iterate();
    __perf_counter_end(&_sort_pc);
    /* __perf_str(_sort_pc.ticks, (void *)lwram); */

    vdp1_cmdt_t * const last_cmdt = render->sort_cmdt;

    /* Set to return from subroutine */
    vdp1_cmdt_link_type_set(last_cmdt, VDP1_CMDT_LINK_TYPE_JUMP_RETURN);

    vdp1_sync_cmdt_put(render->cmdts_pool, cmdt_count, render->sort_link);

    __light_gst_put();

    _render_reset();
}

void
render_debug_log(char *buffer __unused, size_t len __unused)
{
#if CONFIG_MIC3D_LOGGING == 1
/* TODO: Implement */
#endif
}

void
__render_single(const sort_single_t *single)
{
    render_t * const render = __state.render;
    sort_t * const sort = __state.sort;

    /* We can calculate the index (link) just from the element index of the
     * single in the single pool because we always allocate command tables and
     * singles linearly, from their respective pools.
     *
     * This implementation will be an issue should we implement the ability for
     * users to insert their command tables into the sort */
    const vdp1_link_t link = single - (sort->singles_pool + 1);

    vdp1_cmdt_link_set(render->sort_cmdt, link + render->sort_link);

    /* Point to the next command table */
    render->sort_cmdt = &render->cmdts_pool[link];
}

static void
_render_reset(void)
{
    _cmdts_reset();
    __sort_reset();
}

static void
_vdp1_init(void)
{
    static const int16_vec2_t system_clip_coord =
        INT16_VEC2_INITIALIZER(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);

    static const int16_vec2_t local_coord_center =
        INT16_VEC2_INITIALIZER(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    static const vdp1_cmdt_draw_mode_t polygon_draw_mode = {
        .pre_clipping_disable = true
    };

    static int16_vec2_t polygon_points[] = {
        INT16_VEC2_INITIALIZER(-SCREEN_WIDTH / 2     ,                        0),
        INT16_VEC2_INITIALIZER((SCREEN_WIDTH / 2) - 1,                        0),
        INT16_VEC2_INITIALIZER((SCREEN_WIDTH / 2) - 1, (SCREEN_HEIGHT / 2) -  1),
        INT16_VEC2_INITIALIZER(-SCREEN_WIDTH / 2     , (SCREEN_HEIGHT / 2) -  1)
    };
    /*
     * +------+------+-------------+--------------+
     * |      | Horz | # pixels in | # rasters in |
     * |      |      | 1 raster    | 1 field      |
     * +------+------+-------------+--------------+
     * | NTSC | 320  | 1708        | 263          |
     * | PAL  | 352  | 1820        | 313          |
     * +------+------+-------------+--------------+
     *
     * w=352
     * h=224
     * Pr1=1820
     * Pf1=263
     * T=w*h
     * R=(Pr1-200)*(Pf1-h)
     * H=round((T-R)/h)
     *
     * Calculate this via a look up. Skip the clear polygon if there is no
     * need for it */

    /* XXX: This is hardcoded for 352x224 */
    polygon_points[0].y = 70;
    polygon_points[1].y = 70;

    vdp1_cmdt_t * const cmdts = (vdp1_cmdt_t *)VDP1_CMD_TABLE(0, 0);

    vdp1_cmdt_system_clip_coord_set(&cmdts[ORDER_SYSTEM_CLIP_COORDS_INDEX]);
    vdp1_cmdt_vtx_system_clip_coord_set(&cmdts[ORDER_SYSTEM_CLIP_COORDS_INDEX],
        system_clip_coord);

    vdp1_cmdt_local_coord_set(&cmdts[ORDER_LOCAL_COORDS_INDEX]);
    vdp1_cmdt_vtx_local_coord_set(&cmdts[ORDER_LOCAL_COORDS_INDEX],
        local_coord_center);

    vdp1_cmdt_polygon_set(&cmdts[ORDER_CLEAR_POLYGON_INDEX]);
    vdp1_cmdt_draw_mode_set(&cmdts[ORDER_CLEAR_POLYGON_INDEX],
        polygon_draw_mode);
    vdp1_cmdt_color_set(&cmdts[ORDER_CLEAR_POLYGON_INDEX], RGB1555(0, 0, 0, 0));
    vdp1_cmdt_vtx_set(&cmdts[ORDER_CLEAR_POLYGON_INDEX], polygon_points);

    vdp1_cmdt_end_set(&cmdts[ORDER_DRAW_END_INDEX]);
}

static void
_perspective_transform(void)
{
    render_t * const render = __state.render;

    const fix16_mat43_t * const world_matrix = render->world_matrix;
    fix16_mat43_t * const view_matrix = render->view_matrix;
    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    cpu_cache_purge();

    fix16_mat43_mul(camera_matrix, world_matrix, view_matrix);

    const fix16_vec3_t * const m0 = &view_matrix->rotation.row[0];
    const fix16_vec3_t * const m1 = &view_matrix->rotation.row[1];
    const fix16_vec3_t * const m2 = &view_matrix->rotation.row[2];

    const fix16_vec3_t * const points = render->mesh->points;
    int16_vec2_t * const screen_points = render->screen_points_pool;
    int16_t * const z_values = render->z_values_pool;
    /* fix16_t * const depth_values = render->depth_values_pool; */

    for (uint32_t i = 0; i < render->mesh->points_count; i++) {
        fix16_vec3_t p;

        p.z = fix16_vec3_dot(m2, &points[i]) + view_matrix->translation.z;

        cpu_divu_fix16_set(render->view_distance, p.z);

        p.x = fix16_vec3_dot(m0, &points[i]) + view_matrix->translation.x;
        p.y = fix16_vec3_dot(m1, &points[i]) + view_matrix->translation.y;

        const fix16_t depth_value = cpu_divu_quotient_get();

        screen_points[i].x = fix16_high_mul(depth_value, p.x);
        screen_points[i].y = fix16_high_mul(depth_value, p.y);
        z_values[i] = _depth_normalize(p.z);
        /* depth_values[i] = depth_value; */
    }
}

static void
_orthographic_transform(void)
{
    render_t * const render = __state.render;

    const fix16_mat43_t * const world_matrix = render->world_matrix;
    fix16_mat43_t * const view_matrix = render->view_matrix;
    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    cpu_cache_purge();

    fix16_mat43_mul(camera_matrix, world_matrix, view_matrix);

    const fix16_vec3_t * const points = render->mesh->points;
    int16_vec2_t * const screen_points = render->screen_points_pool;
    int16_t * const z_values = render->z_values_pool;
    /* fix16_t * const depth_values = render->depth_values_pool; */

    for (uint32_t i = 0; i < render->mesh->points_count; i++) {
        fix16_vec3_t p;
        fix16_mat43_pos3_mul(view_matrix, &points[i], &p);

        /* TODO: Combine screen_points and z_values pool */
        screen_points[i].x = fix16_high_mul( render->ortho_size, p.x);
        screen_points[i].y = fix16_high_mul(-render->ortho_size, p.y);

        z_values[i] = _depth_normalize(p.z);
        /* depth_values[i] = depth_value; */
    }
}

static inline int32_t
_depth_normalize(fix16_t z)
{
    render_t * const render = __state.render;

    const fix16_t scaled_z =
      fix16_mul(render->depth_scale, z) + render->depth_offset;

    return fix16_high_mul(render->sort_scale, scaled_z);
}

static int16_t
_depth_min_calculate(const int16_t *z_values)
{
    return min(min(z_values[0], z_values[1]),
               min(z_values[2], z_values[3]));
}

static int16_t
_depth_max_calculate(const int16_t *z_values)
{
    return max(max(z_values[0], z_values[1]),
               max(z_values[2], z_values[3]));
}

static int16_t
_depth_center_calculate(const int16_t *z_values)
{
    return ((z_values[0] + z_values[2]) >> 1);
}

static void
_clip_flags_lrtb_calculate(const int16_vec2_t screen_point,
    clip_flags_t *clip_flag)
{
    *clip_flag  = (screen_point.x <   SCREEN_CLIP_LEFT) << CLIP_BIT_LEFT;
    *clip_flag |= (screen_point.x >  SCREEN_CLIP_RIGHT) << CLIP_BIT_RIGHT;
    /* Remember, -Y up so this is why the top and bottom clip flags are
     * reversed */
    *clip_flag |= (screen_point.y <    SCREEN_CLIP_TOP) << CLIP_BIT_TOP;
    *clip_flag |= (screen_point.y > SCREEN_CLIP_BOTTOM) << CLIP_BIT_BOTTOM;
}

static bool
_pipeline_backface_cull_test(void)
{
    render_t * const render = __state.render;
    pipeline_t * const pipeline = render->pipeline;

    const int32_vec2_t a = {
        .x = pipeline->screen_points[2].x - pipeline->screen_points[0].x,
        .y = pipeline->screen_points[2].y - pipeline->screen_points[0].y
    };

    const int32_vec2_t b = {
        .x = pipeline->screen_points[1].x - pipeline->screen_points[0].x,
        .y = pipeline->screen_points[1].y - pipeline->screen_points[0].y
    };

    const int32_t z = (a.x * b.y) - (a.y * b.x);

    return (z <= 0);
}

static void
_clip_flags_calculate(const int16_vec2_t *screen_points,
    clip_flags_t *clip_flags, clip_flags_t *and_flags, clip_flags_t *or_flags)
{
    _clip_flags_lrtb_calculate(screen_points[0], &clip_flags[0]);
    _clip_flags_lrtb_calculate(screen_points[1], &clip_flags[1]);
    _clip_flags_lrtb_calculate(screen_points[2], &clip_flags[2]);
    _clip_flags_lrtb_calculate(screen_points[3], &clip_flags[3]);

    *and_flags = clip_flags[0] & clip_flags[1] & clip_flags[2] & clip_flags[3];
    *or_flags  = clip_flags[0] | clip_flags[1] | clip_flags[2] | clip_flags[3];
}

static void
_indices_swap(uint32_t i, uint32_t j)
{
    render_t * const render = __state.render;
    pipeline_t * const pipeline = render->pipeline;

    const uint16_t tmp = pipeline->polygon.indices.p[i];

    pipeline->polygon.indices.p[i] = pipeline->polygon.indices.p[j];
    pipeline->polygon.indices.p[j] = tmp;
}

static void
_screen_points_swap(int16_vec2_t *screen_points, uint32_t i, uint32_t j)
{
    const int16_vec2_t tmp = screen_points[i];

    screen_points[i] = screen_points[j];
    screen_points[j] = tmp;
}

static void
_pipeline_polygon_orient(void)
{
    render_t * const render = __state.render;
    pipeline_t * const pipeline = render->pipeline;

    /* Orient the vertices such that vertex A is always on-screen. Doing this is
     * for performance purposes */

    if ((pipeline->clip_flags[0] & CLIP_FLAGS_LR) != CLIP_FLAGS_NONE) {
        /* B-|-A
         * | | |
         * C-|-D
         *   |
         *   | Outside
         *
         * Swap A & B
         * Swap D & C */

        _indices_swap(0, 1);
        _indices_swap(3, 2);

        _screen_points_swap(pipeline->screen_points, 0, 1);
        _screen_points_swap(pipeline->screen_points, 2, 3);

        pipeline->attribute.control.raw ^= VDP1_CMDT_CHAR_FLIP_H;
    }

    if ((pipeline->clip_flags[0] & CLIP_FLAGS_TB) != CLIP_FLAGS_NONE) {
        /*   B---A Outside
         * --|---|--
         *   C---D
         *
         *   Swap A & D
         *   Swap B & C */

        _indices_swap(0, 3);
        _indices_swap(1, 2);

        _screen_points_swap(pipeline->screen_points, 0, 3);
        _screen_points_swap(pipeline->screen_points, 1, 2);

        pipeline->attribute.control.raw ^= VDP1_CMDT_CHAR_FLIP_V;
    }
}

static uint32_t
_cmdts_count_get(void)
{
    render_t * const render = __state.render;

    return (render->cmdts - render->cmdts_pool);
}

static vdp1_cmdt_t *
_cmdts_alloc(void)
{
    render_t * const render = __state.render;

    vdp1_cmdt_t * const cmdt = render->cmdts;

    render->cmdts++;

    /* XXX: Assert that we don't exceed the alloted command table count */

    return cmdt;
}

static void
_cmdts_reset(void)
{
    render_t * const render = __state.render;

    render->cmdts = render->cmdts_pool;
}

static void
_cmdt_process(vdp1_cmdt_t *cmdt)
{
    render_t * const render = __state.render;
    pipeline_t * const pipeline = render->pipeline;

    cmdt->cmd_ctrl = pipeline->attribute.control.raw;
    cmdt->cmd_pmod = pipeline->attribute.draw_mode.raw;

    if (pipeline->polygon.flags.use_texture) {
        const texture_t * const textures = tlist_get();
        const texture_t * const texture =
            &textures[pipeline->attribute.texture_slot];

        cmdt->cmd_srca = texture->vram_index;
        cmdt->cmd_size = texture->size;
    }

    cmdt->cmd_colr = pipeline->attribute.palette_data.raw;

    cmdt->cmd_vertices[0] = pipeline->screen_points[0];
    cmdt->cmd_vertices[1] = pipeline->screen_points[1];
    cmdt->cmd_vertices[2] = pipeline->screen_points[2];
    cmdt->cmd_vertices[3] = pipeline->screen_points[3];

    cmdt->cmd_grda = pipeline->attribute.shading_slot;
}

static void
_cmdts_insert(vdp1_cmdt_t *cmdt)
{
    /* Required */
    vdp1_cmdt_link_type_set(cmdt, VDP1_CMDT_LINK_TYPE_JUMP_ASSIGN);
}

static void
_cpu_divu_ovfi_handler(void)
{
    assert(false);
}

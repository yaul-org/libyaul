/*
 * Copyright (c) 2006-2023
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Mic
 * Shazz
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cpu/registers.h>
#include <cpu/divu.h>
#include <cpu/intc.h>

#include "internal.h"

#define SCREEN_RATIO       FIX16(SCREEN_WIDTH / (float)SCREEN_HEIGHT)

#define SCREEN_CLIP_LEFT   (-SCREEN_WIDTH / 2)
#define SCREEN_CLIP_RIGHT  ( SCREEN_WIDTH / 2)
#define SCREEN_CLIP_TOP    (-SCREEN_HEIGHT / 2)
#define SCREEN_CLIP_BOTTOM ( SCREEN_HEIGHT / 2)

#define MIN_FOV_ANGLE DEG2ANGLE( 20.0f)
#define MAX_FOV_ANGLE DEG2ANGLE(120.0f)

#define NEAR_LEVEL_MIN 1U
#define NEAR_LEVEL_MAX 8U

#define ORDER_SYSTEM_CLIP_COORDS_INDEX  0
#define ORDER_SUBR_INDEX                1
#define ORDER_CLEAR_POLYGON_INDEX       1
#define ORDER_LOCAL_COORDS_INDEX        2
#define ORDER_DRAW_END_INDEX            3
#define ORDER_INDEX                     4

static void _reset(void);

static void _vdp1_init(void);

static void _transform(void);

static fix16_t _depth_calculate(void);
static fix16_t _depth_min_calculate(const fix16_t *z_values);
static fix16_t _depth_max_calculate(const fix16_t *z_values);
static fix16_t _depth_center_calculate(const fix16_t *z_values);

static bool _backface_cull_test(void);

static void _clip_flags_calculate(void);
static void _clip_flags_lrtb_calculate(const int16_vec2_t screen_point, clip_flags_t *clip_flag);

static void _polygon_orient(void);

static void _render_single(const sort_single_t *single);

static vdp1_cmdt_t *_cmdts_alloc(void);
static void _cmdts_reset(void);
static vdp1_link_t _cmdt_link_calculate(const vdp1_cmdt_t *cmdt);
static void _cmdt_process(vdp1_cmdt_t *cmdt);

static perf_counter_t _transform_pc __unused;

void
__render_init(void)
{
        extern fix16_t __pool_z_values[];
        extern int16_vec2_t __pool_screen_points[];
        extern fix16_t __pool_depth_values[];

        extern vdp1_cmdt_t __pool_cmdts[];

        extern render_transform_t __render_transform;

        render_t * const render = __state.render;

        render->z_values_pool = __pool_z_values;
        render->screen_points_pool = __pool_screen_points;
        render->depth_values_pool = __pool_depth_values;
        render->cmdts_pool = __pool_cmdts;

        render->mesh = NULL;
        render->render_transform = &__render_transform;

        render->render_flags = RENDER_FLAGS_NONE;

        render_perspective_set(DEG2ANGLE(90.0f));
        render_near_level_set(7);
        render_far_set(FIX16(1024));

        _reset();

        __perf_counter_init(&_transform_pc);

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
render_perspective_set(angle_t fov_angle)
{
        render_t * const render = __state.render;

        fov_angle = clamp(fov_angle, MIN_FOV_ANGLE, MAX_FOV_ANGLE);

        const angle_t hfov_angle = fov_angle >> 1;
        const fix16_t screen_scale = FIX16(0.5f * (SCREEN_WIDTH - 1));
        const fix16_t tan = fix16_tan(hfov_angle);

        render->view_distance = fix16_mul(screen_scale, tan);
}

void
render_near_level_set(uint32_t level)
{
        render_t * const render = __state.render;

        render->near = render->view_distance;

        const uint32_t clamped_level =
            clamp(level + 1, NEAR_LEVEL_MIN, NEAR_LEVEL_MAX);

        for (int32_t i = clamped_level; i > 0; i--) {
                render->near >>= 1;
        }
}

void
render_far_set(fix16_t far)
{
        render_t * const render = __state.render;

        render->far = fix16_clamp(far, render->near, FIX16(2048.0f));
        render->sort_scale = fix16_div(FIX16(SORT_DEPTH - 1), render->far);
}

void
render_mesh_transform(const mesh_t *mesh)
{
        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        render->mesh = mesh;

        __perf_counter_start(&_transform_pc);

        _transform();

        __light_transform();

        fix16_t * const z_values = render->z_values_pool;
        int16_vec2_t * const screen_points = render->screen_points_pool;
        const polygon_t * const polygons = render->mesh->polygons;

        for (uint32_t i = 0; i < render->mesh->polygons_count; i++) {
                render_transform->screen_points[0] = screen_points[polygons[i].indices.p0];
                render_transform->screen_points[1] = screen_points[polygons[i].indices.p1];
                render_transform->screen_points[2] = screen_points[polygons[i].indices.p2];
                render_transform->screen_points[3] = screen_points[polygons[i].indices.p3];

                render_transform->ro_attribute = &render->mesh->attributes[i];
                render_transform->rw_attribute.control = render_transform->ro_attribute->control;

                if (render_transform->rw_attribute.control.plane_type != PLANE_TYPE_DOUBLE) {
                        if ((_backface_cull_test())) {
                                continue;
                        }
                }

                render_transform->rw_attribute.draw_mode = render_transform->ro_attribute->draw_mode;

                render_transform->z_values[0] = z_values[polygons[i].indices.p0];
                render_transform->z_values[1] = z_values[polygons[i].indices.p1];
                render_transform->z_values[2] = z_values[polygons[i].indices.p2];
                render_transform->z_values[3] = z_values[polygons[i].indices.p3];

                const fix16_t depth_z = _depth_calculate();

                /* Cull polygons intersecting with the near plane */
                if ((depth_z < render->near)) {
                        continue;
                }

                _clip_flags_calculate();

                /* Cull if the polygon is entirely off screen */
                if (render_transform->and_flags != CLIP_FLAGS_NONE) {
                        continue;
                }

                render_transform->indices = polygons[i].indices;

                if (render_transform->or_flags == CLIP_FLAGS_NONE) {
                        /* If no clip flags are set, disable pre-clipping. This
                         * should help with performance */
                        render_transform->rw_attribute.draw_mode.pre_clipping_disable = true;
                } else {
                        _polygon_orient();
                }

                __light_polygon_process();

                vdp1_cmdt_t * const cmdt = _cmdts_alloc();
                const vdp1_link_t cmdt_link = _cmdt_link_calculate(cmdt);

                _cmdt_process(cmdt);

                const int32_t scaled_z = fix16_int32_mul(depth_z, render->sort_scale);

                __sort_insert(cmdt_link, scaled_z);

                render->cmdt_count++;
        }

        __perf_counter_end(&_transform_pc);

        char buffer[32] __unused;
        __perf_str(_transform_pc.ticks, buffer);

        /* dbgio_printf("%lu\n", render->cmdt_count); */
        /* dbgio_printf("ticks: %5lu, %5lu, %sms\n", _transform_pc.ticks, _transform_pc.max_ticks, buffer); */

        cpu_intc_mask_set(sr_mask);
}

void
render(void)
{
        render_t * const render = __state.render;

        vdp1_cmdt_t * const subr_cmdt = (vdp1_cmdt_t *)VDP1_CMD_TABLE(ORDER_SUBR_INDEX, 0);

        if (render->cmdt_count == 0) {
                vdp1_cmdt_link_type_set(subr_cmdt, VDP1_CMDT_LINK_TYPE_JUMP_NEXT);

                return;
        }

        render->sort_cmdt = subr_cmdt;
        render->sort_link = ORDER_INDEX;

        /* Set as a subroutine call. If RENDER_FLAGS_NO_CLEAR is set, skip the
         * clear polygon (ORDER_SUBR_INDEX), but start the call */
        if (RENDER_FLAG_TEST(NO_CLEAR)) {
                vdp1_cmdt_link_type_set(subr_cmdt, VDP1_CMDT_LINK_TYPE_SKIP_CALL);

                vdp1_sync_mode_set(VDP1_SYNC_MODE_CHANGE_ONLY);
        } else {
                vdp1_cmdt_link_type_set(subr_cmdt, VDP1_CMDT_LINK_TYPE_JUMP_CALL);

                vdp1_sync_mode_set(VDP1_SYNC_MODE_ERASE_CHANGE);
        }

        __sort_iterate(_render_single);

        vdp1_cmdt_t * const end_cmdt = render->sort_cmdt;

        /* Set to return from subroutine */
        vdp1_cmdt_link_type_set(end_cmdt, VDP1_CMDT_LINK_TYPE_JUMP_RETURN);

        vdp1_sync_cmdt_put(render->cmdts_pool, render->cmdt_count, render->sort_link);

        __light_gst_put();

        _reset();
}

static void
_reset(void)
{
        _cmdts_reset();
        __sort_start();
}

static void
_vdp1_init(void)
{
        static const int16_vec2_t system_clip_coord =
            INT16_VEC2_INITIALIZER(SCREEN_WIDTH - 1,
                                   SCREEN_HEIGHT - 1);

        static const int16_vec2_t local_coord_center =
            INT16_VEC2_INITIALIZER(SCREEN_WIDTH / 2,
                                   SCREEN_HEIGHT / 2);

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
        vdp1_cmdt_vtx_system_clip_coord_set(&cmdts[ORDER_SYSTEM_CLIP_COORDS_INDEX], system_clip_coord);

        vdp1_cmdt_polygon_set(&cmdts[ORDER_CLEAR_POLYGON_INDEX]);
        vdp1_cmdt_draw_mode_set(&cmdts[ORDER_CLEAR_POLYGON_INDEX], polygon_draw_mode);
        vdp1_cmdt_color_set(&cmdts[ORDER_CLEAR_POLYGON_INDEX], RGB1555(0, 0, 0, 0));
        vdp1_cmdt_vtx_set(&cmdts[ORDER_CLEAR_POLYGON_INDEX], polygon_points);

        vdp1_cmdt_local_coord_set(&cmdts[ORDER_LOCAL_COORDS_INDEX]);
        vdp1_cmdt_vtx_local_coord_set(&cmdts[ORDER_LOCAL_COORDS_INDEX], local_coord_center);

        vdp1_cmdt_end_set(&cmdts[ORDER_DRAW_END_INDEX]);
}

static void
_transform(void)
{
        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        const fix16_mat43_t * const world_matrix = matrix_top();

        fix16_mat43_t inv_view_matrix __aligned(16);

        __camera_view_invert(&inv_view_matrix);

        fix16_mat43_mul(&inv_view_matrix, world_matrix, &render_transform->view_matrix);

        const fix16_vec3_t * const m0 = (const fix16_vec3_t *)&render_transform->view_matrix.row[0];
        const fix16_vec3_t * const m1 = (const fix16_vec3_t *)&render_transform->view_matrix.row[1];
        const fix16_vec3_t * const m2 = (const fix16_vec3_t *)&render_transform->view_matrix.row[2];

        const fix16_vec3_t * const points = render->mesh->points;
        int16_vec2_t * const screen_points = render->screen_points_pool;
        fix16_t * const z_values = render->z_values_pool;
        /* fix16_t * const depth_values = render->depth_values_pool; */

        for (uint32_t i = 0; i < render->mesh->points_count; i++) {
                const fix16_t z = fix16_vec3_dot(m2, &points[i]) + render_transform->view_matrix.frow[2][3];
                const fix16_t clamped_z = fix16_max(z, render->near);

                cpu_divu_fix16_set(render->view_distance, clamped_z);

                const fix16_t x = fix16_vec3_dot(m0, &points[i]) + render_transform->view_matrix.frow[0][3];
                const fix16_t y = fix16_vec3_dot(m1, &points[i]) + render_transform->view_matrix.frow[1][3];

                const fix16_t depth_value = cpu_divu_quotient_get();

                screen_points[i].x = fix16_int32_mul(depth_value, x);
                screen_points[i].y = fix16_int32_mul(depth_value, y);
                z_values[i] = z;
                /* depth_values[i] = depth_value; */
        }
}

static fix16_t
_depth_calculate(void)
{
        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        switch (render_transform->rw_attribute.control.sort_type) {
        default:
        case SORT_TYPE_CENTER:
                return _depth_center_calculate(render_transform->z_values);
        case SORT_TYPE_MIN:
                return _depth_min_calculate(render_transform->z_values);
        case SORT_TYPE_MAX:
                return _depth_max_calculate(render_transform->z_values);
        }
}

static fix16_t
_depth_min_calculate(const fix16_t *z_values)
{
        return fix16_min(fix16_min(z_values[0], z_values[1]),
                         fix16_min(z_values[2], z_values[3]));
}

static fix16_t
_depth_max_calculate(const fix16_t *z_values)
{
        return fix16_max(fix16_max(z_values[0], z_values[1]),
                         fix16_max(z_values[2], z_values[3]));
}

static fix16_t
_depth_center_calculate(const fix16_t *z_values)
{
        return ((z_values[0] + z_values[2]) >> 1);
}

static void
_clip_flags_lrtb_calculate(const int16_vec2_t screen_point, clip_flags_t *clip_flag)
{
        *clip_flag  = (screen_point.x <   SCREEN_CLIP_LEFT) << CLIP_BIT_LEFT;
        *clip_flag |= (screen_point.x >  SCREEN_CLIP_RIGHT) << CLIP_BIT_RIGHT;
        /* Remember, -Y up so this is why the top and bottom clip flags are
         * reversed */
        *clip_flag |= (screen_point.y <    SCREEN_CLIP_TOP) << CLIP_BIT_TOP;
        *clip_flag |= (screen_point.y > SCREEN_CLIP_BOTTOM) << CLIP_BIT_BOTTOM;
}

static bool
_backface_cull_test(void)
{
        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        const int32_vec2_t a = {
                .x = render_transform->screen_points[2].x - render_transform->screen_points[0].x,
                .y = render_transform->screen_points[2].y - render_transform->screen_points[0].y
        };

        const int32_vec2_t b = {
                .x = render_transform->screen_points[1].x - render_transform->screen_points[0].x,
                .y = render_transform->screen_points[1].y - render_transform->screen_points[0].y
        };

        const int32_t z = (a.x * b.y) - (a.y * b.x);

        return (z < 0);
}

static void
_clip_flags_calculate(void)
{
        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        _clip_flags_lrtb_calculate(render_transform->screen_points[0], &render_transform->clip_flags[0]);
        _clip_flags_lrtb_calculate(render_transform->screen_points[1], &render_transform->clip_flags[1]);
        _clip_flags_lrtb_calculate(render_transform->screen_points[2], &render_transform->clip_flags[2]);
        _clip_flags_lrtb_calculate(render_transform->screen_points[3], &render_transform->clip_flags[3]);

        render_transform->and_flags = render_transform->clip_flags[0] &
                                      render_transform->clip_flags[1] &
                                      render_transform->clip_flags[2] &
                                      render_transform->clip_flags[3];

        render_transform->or_flags = render_transform->clip_flags[0] |
                                     render_transform->clip_flags[1] |
                                     render_transform->clip_flags[2] |
                                     render_transform->clip_flags[3];
}

static void
_indices_swap(uint32_t i, uint32_t j)
{
        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        const uint16_t tmp = render_transform->indices.p[i];

        render_transform->indices.p[i] = render_transform->indices.p[j];
        render_transform->indices.p[j] = tmp;
}

static void
_screen_points_swap(uint32_t i, uint32_t j)
{
        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        const int16_vec2_t tmp = render_transform->screen_points[i];

        render_transform->screen_points[i] = render_transform->screen_points[j];
        render_transform->screen_points[j] = tmp;
}

static void
_polygon_orient(void)
{
        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        /* Orient the vertices such that vertex A is always on-screen. Doing
         * this is for performance purposes */

        if ((render_transform->clip_flags[0] & CLIP_FLAGS_LR) != CLIP_FLAGS_NONE) {
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

                _screen_points_swap(0, 1);
                _screen_points_swap(2, 3);

                render_transform->rw_attribute.control.raw ^= VDP1_CMDT_CHAR_FLIP_H;
        }

        if ((render_transform->clip_flags[0] & CLIP_FLAGS_TB) != CLIP_FLAGS_NONE) {
                /*   B---A Outside
                 * --|---|--
                 *   C---D
                 *
                 *   Swap A & D
                 *   Swap B & C */

                _indices_swap(0, 3);
                _indices_swap(1, 2);

                _screen_points_swap(0, 3);
                _screen_points_swap(1, 2);

                render_transform->rw_attribute.control.raw ^= VDP1_CMDT_CHAR_FLIP_V;
        }
}

static void
_render_single(const sort_single_t *single)
{
        render_t * const render = __state.render;

        vdp1_cmdt_link_set(render->sort_cmdt, single->link + render->sort_link);

        /* Point to the next command table */
        render->sort_cmdt = &render->cmdts_pool[single->link];
}

static vdp1_cmdt_t *
_cmdts_alloc(void)
{
        render_t * const render = __state.render;

        vdp1_cmdt_t * const cmdt = render->cmdts;

        render->cmdts++;

        return cmdt;
}

static void
_cmdts_reset(void)
{
        render_t * const render = __state.render;

        render->cmdts = render->cmdts_pool;
        render->cmdt_count = 0;
}

static vdp1_link_t
_cmdt_link_calculate(const vdp1_cmdt_t *cmdt)
{
        render_t * const render = __state.render;

        return (cmdt - render->cmdts_pool);
}

static void
_cmdt_process(vdp1_cmdt_t *cmdt)
{
        render_t * const render = __state.render;
        render_transform_t * const render_transform = render->render_transform;

        cmdt->cmd_ctrl = VDP1_CMDT_LINK_TYPE_JUMP_ASSIGN | (render_transform->rw_attribute.control.raw & 0x3F);
        cmdt->cmd_pmod = render_transform->rw_attribute.draw_mode.raw;

        if (render_transform->rw_attribute.control.use_texture) {
                const texture_t * const textures = tlist_get();
                const texture_t * const texture = &textures[render_transform->ro_attribute->texture_slot];

                cmdt->cmd_srca = texture->vram_index;
                cmdt->cmd_size = texture->size;
        }

        cmdt->cmd_colr = render_transform->ro_attribute->palette.raw;

        cmdt->cmd_vertices[0] = render_transform->screen_points[0];
        cmdt->cmd_vertices[1] = render_transform->screen_points[1];
        cmdt->cmd_vertices[2] = render_transform->screen_points[2];
        cmdt->cmd_vertices[3] = render_transform->screen_points[3];

        cmdt->cmd_grda = render_transform->rw_attribute.shading_slot;
}

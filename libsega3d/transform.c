/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/divu.h>
#include <vdp.h>

#include <dbgio.h>

#include "sega3d-internal.h"

extern void _internal_sort_clear(void);
extern void _internal_sort_add(void *packet, int32_t pz);
extern void _internal_sort_iterate(iterate_fn fn);

static bool _object_aabb_cull_test(const transform_t * const trans) __unused;
static bool _object_sphere_cull_test(const transform_t * const trans);
static bool _screen_cull_test(const transform_t * const trans);
static void _camera_world_transform(void);
static void _cmdt_prepare(const transform_t * const trans);
static void _fog_calculate(const transform_t * const trans);
static void _polygon_process(transform_t * const trans, POLYGON const *polygons);
static void _sort_iterate(const sort_single_t *single);
static void _vertex_pool_clipping(const transform_t * const trans);
static void _vertex_pool_transform(const transform_t * const trans, const POINT * const points);
static void _z_calculate(transform_t * const trans);

static inline FIXED __always_inline __unused
_point_component_transform(const fix16_vec3_t *p, const FIXED *matrix)
{
        cpu_instr_clrmac();

        const fix16_vec3_t *p_p = p;
        const FIXED *matrix_p = matrix;

        cpu_instr_macl(&p_p, &matrix_p);
        cpu_instr_macl(&p_p, &matrix_p);
        cpu_instr_macl(&p_p, &matrix_p);

        register const uint32_t mach = cpu_instr_sts_mach();
        register const uint32_t macl = cpu_instr_sts_macl();
        register const uint32_t xtrct = cpu_instr_xtrct(mach, macl);

        return (xtrct + (*matrix_p));
}

static inline fix16_vec3_t __always_inline __unused
_point_transform(const fix16_vec3_t *p, const FIXED *matrix)
{
        fix16_vec3_t out_p;

        out_p.x = _point_component_transform(p, &matrix[M00]);
        out_p.y = _point_component_transform(p, &matrix[M10]);
        out_p.z = _point_component_transform(p, &matrix[M20]);

        return out_p;
}

static inline FIXED __always_inline __unused
_normal_component_rotate(const fix16_vec3_t *p, const FIXED *matrix)
{
        cpu_instr_clrmac();

        const fix16_vec3_t *p_p = p;
        const FIXED *matrix_p = matrix;

        cpu_instr_macl(&p_p, &matrix_p);
        cpu_instr_macl(&p_p, &matrix_p);
        cpu_instr_macl(&p_p, &matrix_p);

        register const uint32_t mach = cpu_instr_sts_mach();
        register const uint32_t macl = cpu_instr_sts_macl();
        register const uint32_t xtrct = cpu_instr_xtrct(mach, macl);

        return xtrct;
}

static inline fix16_vec3_t __always_inline __unused
_normal_rotate(const fix16_vec3_t *p, const FIXED *matrix)
{
        fix16_vec3_t out_p;

        out_p.x = _normal_component_rotate(p, &matrix[M00]);
        out_p.y = _normal_component_rotate(p, &matrix[M10]);
        out_p.z = _normal_component_rotate(p, &matrix[M20]);

        return out_p;
}

static vdp1_cmdt_t _cmdt_end;

void
_internal_transform_init(void)
{
        vdp1_cmdt_end_set(&_cmdt_end);

        sega3d_matrix_identity(_internal_state->clip_camera);
}

void
sega3d_start(vdp1_cmdt_orderlist_t *orderlist, uint16_t orderlist_offset, vdp1_cmdt_t *cmdts)
{
        assert(orderlist != NULL);

        _internal_sort_clear();

        transform_t * const trans = _internal_state->transform;

        trans->orderlist = orderlist;
        trans->current_orderlist = &orderlist[orderlist_offset];
        trans->current_cmdt = cmdts;

        sega3d_results_t * const results = _internal_state->results;

        results->object_count = 0;
        results->polygon_count = 0;

        const FIXED * const camera_matrix =
            (const FIXED *)_internal_state->clip_camera;

        clip_planes_t * const clip_planes = _internal_state->clip_planes;
        fix16_plane_t * const out_clip_planes =
            (fix16_plane_t *)_internal_state->clip_planes;
        const fix16_vec3_t * const clip_normals =
            (const fix16_vec3_t *)&_internal_state->clip_planes->near_normal;

        for (uint32_t i = 0; i < 6; i++) {
                fix16_plane_t * const clip_plane = &out_clip_planes[i];
                const fix16_vec3_t * const clip_normal = &clip_normals[i];

                clip_plane->normal = _normal_rotate(clip_normal, camera_matrix);
        }

        /* Both the near and far plane point needs to be rotated, whereas the
         * other planes all can use the camera's position as the point on the
         * plane since all the other planes intersect at that point.
         *
         * This however can have overflow issues when the objects are really far
         * away from the frustum */

        clip_planes->near_plane.d =
            _point_transform(&clip_planes->near_d, camera_matrix);
        clip_planes->far_plane.d =
            _point_transform(&clip_planes->far_d, camera_matrix);

        for (uint32_t i = 2; i < CLIP_PLANE_COUNT; i++) {
                fix16_plane_t * const clip_plane = &out_clip_planes[i];

                clip_plane->d.x = camera_matrix[M03];
                clip_plane->d.y = camera_matrix[M13];
                clip_plane->d.z = camera_matrix[M23];
        }
}

void
sega3d_finish(sega3d_results_t *results)
{
        _internal_sort_iterate(_sort_iterate);

        transform_t * const trans = _internal_state->transform;

        /* Fetch the last command table pointer before setting the indirect mode
         * transfer end bit */
        trans->current_orderlist->cmdt = &_cmdt_end;

        vdp1_cmdt_orderlist_end(trans->current_orderlist);

        vdp1_sync_cmdt_orderlist_put(trans->orderlist);

        if (results != NULL) {
                sega3d_results_t * const internal_results = _internal_state->results;

                results->object_count = internal_results->object_count;
                results->polygon_count = trans->current_orderlist - trans->orderlist;
        }
}

void
sega3d_object_transform(const sega3d_object_t *object, uint16_t xpdata_index)
{
        const XPDATA * const object_xpdata = object->xpdatas;
        const XPDATA * const xpdata = &object_xpdata[xpdata_index];

        const uint16_t polygon_count =
            (xpdata->nbPolygon < (PACKET_SIZE - 1)) ? xpdata->nbPolygon : (PACKET_SIZE - 1);
        const uint16_t vertex_count =
            (xpdata->nbPoint < VERTEX_POOL_SIZE) ? xpdata->nbPoint : VERTEX_POOL_SIZE;

        if ((vertex_count == 0) || (polygon_count == 0)) {
                return;
        }

        transform_t * const trans = _internal_state->transform;

        trans->object = object;
        trans->xpdata = xpdata;
        trans->vertex_count = vertex_count;
        trans->polygon_count = polygon_count;

        if ((object->flags & SEGA3D_OBJECT_FLAGS_CULL_AABB) != SEGA3D_OBJECT_FLAGS_NONE) {
                if ((_object_aabb_cull_test(trans))) {
                        return;
                }
        } else if ((object->flags & SEGA3D_OBJECT_FLAGS_CULL_SPHERE) != SEGA3D_OBJECT_FLAGS_NONE) {
                if ((_object_sphere_cull_test(trans))) {
                        return;
                }
        }

        sega3d_matrix_push(SEGA3D_MATRIX_TYPE_PUSH); {
                _camera_world_transform();
                _vertex_pool_transform(trans, xpdata->pntbl);
                _vertex_pool_clipping(trans);
                _polygon_process(trans, xpdata->pltbl);
        } sega3d_matrix_pop();

        sega3d_results_t * const results = _internal_state->results;

        results->object_count++;
}

static void
_camera_world_transform(void)
{
        FIXED * const top_matrix = (FIXED *)sega3d_matrix_top();
        const FIXED * const camera_matrix =
            (const FIXED *)_internal_state->clip_camera;

        top_matrix[M03] += -camera_matrix[M03];
        top_matrix[M13] += -camera_matrix[M13];
        top_matrix[M23] += -camera_matrix[M23];
}

static void
_vertex_pool_transform(const transform_t * const trans, const POINT * const points)
{
#define OFFSET_CPU_DVSR         (0x00)
#define OFFSET_CPU_DVDNTH       (0x10 / 4)
#define OFFSET_CPU_DVDNTL       (0x14 / 4)

        const sega3d_info_t * const info = _internal_state->info;

        const FIXED *current_point = (const FIXED *)points;
        const FIXED * const last_point = (const FIXED * const)&points[trans->vertex_count];

        transform_proj_t *trans_proj;
        trans_proj = &_internal_state->transform_proj_pool[0];

        const FIXED view_distance = info->view_distance;
        const FIXED view_distance_16 = view_distance << 16;
        const FIXED z_near = info->near;
        const FIXED ratio = info->ratio;

        const FIXED * const top_matrix = (const FIXED *)sega3d_matrix_top();
        const FIXED * const matrix = &top_matrix[M20];

        register uint32_t * const cpu_divu_regs = (uint32_t *)CPU(DVSR);

        do {
                cpu_instr_clrmac();

                const FIXED *matrix_p = matrix;

                trans_proj->clip_flags = CLIP_FLAGS_NONE;

                cpu_instr_macl(&current_point, &matrix_p);
                const uint32_t dh1 = cpu_instr_swapw(view_distance);
                cpu_instr_macl(&current_point, &matrix_p);
                const uint32_t dh2 = cpu_instr_extsw(dh1);
                cpu_instr_macl(&current_point, &matrix_p);
                cpu_divu_regs[OFFSET_CPU_DVDNTH] = dh2;

                register const uint32_t z_mach = cpu_instr_sts_mach();

                const uint32_t tz = *matrix_p;

                const uint32_t z_macl = cpu_instr_sts_macl();
                current_point -= XYZ;
                const uint32_t z_xtrct = cpu_instr_xtrct(z_mach, z_macl);

                trans_proj->point_z = (z_xtrct + tz);

                /* In case the projected Z value is on or behind the near plane */
                if (trans_proj->point_z < z_near) {
                        trans_proj->clip_flags |= CLIP_FLAGS_NEAR;

                        trans_proj->point_z = z_near;
                }

                cpu_instr_clrmac();

                matrix_p -= M23; /* Move back to start of matrix */

                cpu_instr_macl(&current_point, &matrix_p);
                cpu_divu_regs[OFFSET_CPU_DVSR] = trans_proj->point_z;
                cpu_instr_macl(&current_point, &matrix_p);
                cpu_divu_regs[OFFSET_CPU_DVDNTL] = view_distance_16;
                cpu_instr_macl(&current_point, &matrix_p);

                const uint32_t x_mach = cpu_instr_sts_mach();
                const uint32_t tx = *matrix_p;
                const uint32_t x_macl = cpu_instr_sts_macl();
                matrix_p++;

                const uint32_t x_xtrct = cpu_instr_xtrct(x_mach, x_macl);

                cpu_instr_clrmac();

                current_point -= XYZ;

                const FIXED point_x = (x_xtrct + tx);

                cpu_instr_macl(&current_point, &matrix_p);
                cpu_instr_macl(&current_point, &matrix_p);
                cpu_instr_macl(&current_point, &matrix_p);

                const uint32_t y_mach = cpu_instr_sts_mach();
                const uint32_t y_macl = cpu_instr_sts_macl();
                const uint32_t ty = *matrix_p;
                const uint32_t y_xtrct = cpu_instr_xtrct(y_mach, y_macl);

                const FIXED point_y = (y_xtrct + ty);

                const FIXED inv_z = cpu_divu_regs[OFFSET_CPU_DVDNTL];

                trans_proj->screen.x = fix16_int16_muls(point_x, inv_z);
                trans_proj->screen.y = fix16_int16_muls(point_y, fix16_mul(ratio, inv_z));

                trans_proj++;
        } while (current_point <= last_point);
}

static void
_sort_iterate(const sort_single_t *single)
{
        transform_t * const trans = _internal_state->transform;

        /* No need to clear the end bit, as setting the "source" clobbers the
         * bit */
        trans->current_orderlist->cmdt = single->packet;
        trans->current_orderlist++;
}

static void
_vertex_pool_clipping(const transform_t * const trans)
{
        transform_proj_t *trans_proj;
        trans_proj = &_internal_state->transform_proj_pool[0];

        const int16_t sw_2 = trans->cached_sw_2;
        const int16_t sw_n2 = -trans->cached_sw_2;
        const int16_t sh_2 = trans->cached_sh_2;
        const int16_t sh_n2 = -trans->cached_sh_2;

        uint16_t vertex_count;
        vertex_count = trans->vertex_count;

        do {
                if (trans_proj->screen.x < sw_n2) {
                        trans_proj->clip_flags |= CLIP_FLAGS_SIDE;
                } else if (trans_proj->screen.x > sw_2) {
                        trans_proj->clip_flags |= CLIP_FLAGS_SIDE;
                } else if (trans_proj->screen.y > sh_2) {
                        trans_proj->clip_flags |= CLIP_FLAGS_SIDE;
                } else if (trans_proj->screen.y < sh_n2) {
                        trans_proj->clip_flags |= CLIP_FLAGS_SIDE;
                }

                trans_proj++;
                vertex_count--;
        } while (vertex_count != 0);
}

static void
_polygon_process(transform_t * const trans, POLYGON const *polygons)
{
        transform_proj_t * const transform_proj_pool =
            &_internal_state->transform_proj_pool[0];

        const sega3d_object_t * const object = trans->object;
        const uint16_t polygon_count = trans->polygon_count;

        for (trans->index = 0; trans->index < polygon_count; trans->index++, polygons++) {
                const uint16_t * vertices = &polygons->Vertices[0];

                trans->polygon[0] = &transform_proj_pool[*(vertices++)];
                trans->polygon[1] = &transform_proj_pool[*(vertices++)];
                trans->polygon[2] = &transform_proj_pool[*(vertices++)];
                trans->polygon[3] = &transform_proj_pool[*vertices];

                const clip_flags_t and_clip_flags = (trans->polygon[0]->clip_flags &
                                                     trans->polygon[1]->clip_flags &
                                                     trans->polygon[2]->clip_flags &
                                                     trans->polygon[3]->clip_flags);

                if (and_clip_flags != CLIP_FLAGS_NONE) {
                        continue;
                }

                if ((object->flags & SEGA3D_OBJECT_FLAGS_CULL_SCREEN) != SEGA3D_OBJECT_FLAGS_NONE) {
                        if ((_screen_cull_test(trans))) {
                                continue;
                        }
                }

                _z_calculate(trans);

                _cmdt_prepare(trans);

                _internal_sort_add(trans->current_cmdt, fix16_int32_to(trans->z_value));

                trans->current_cmdt++;
        }
}

static void
_z_calculate(transform_t * const trans)
{
        const XPDATA * const xpdata = trans->xpdata;
        const ATTR * const attr = &xpdata->attbl[trans->index];

        const uint32_t sort = attr->sort & 0x03; 

        if (sort == SORT_CEN) {
                const FIXED z_avg = trans->polygon[0]->point_z +
                                    trans->polygon[1]->point_z +
                                    trans->polygon[2]->point_z +
                                    trans->polygon[3]->point_z;

                /* Divide by 4 to get the average (bit shift) */
                trans->z_value = z_avg >> 2;
        } else if (sort == SORT_MIN) {
                trans->z_value = trans->polygon[0]->point_z; 

                trans->z_value = (trans->polygon[1]->point_z < trans->z_value)
                    ? trans->polygon[1]->point_z
                    : trans->z_value;
                trans->z_value = (trans->polygon[2]->point_z < trans->z_value)
                    ? trans->polygon[2]->point_z
                    : trans->z_value;
                trans->z_value = (trans->polygon[3]->point_z < trans->z_value)
                    ? trans->polygon[3]->point_z
                    : trans->z_value;
        } else if (sort == SORT_MAX) {
                trans->z_value = trans->polygon[0]->point_z;

                trans->z_value = (trans->polygon[1]->point_z > trans->z_value)
                    ? trans->polygon[1]->point_z
                    : trans->z_value;
                trans->z_value = (trans->polygon[2]->point_z > trans->z_value)
                    ? trans->polygon[2]->point_z
                    : trans->z_value;
                trans->z_value = (trans->polygon[3]->point_z > trans->z_value)
                    ? trans->polygon[3]->point_z
                    : trans->z_value;
        } else if (sort == SORT_BFR) {
                assert(false && "Not yet implemented");
        }
}

static void
_cmdt_prepare(const transform_t * const trans)
{
        const sega3d_object_t * const object = trans->object;
        const XPDATA * const xpdata = trans->xpdata;

        vdp1_cmdt_t * const cmdt = trans->current_cmdt;

        const ATTR *attr;
        attr = &xpdata->attbl[trans->index];

        cmdt->cmd_ctrl = attr->dir; /* We care about (Dir) and (Comm) bits */
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = attr->atrb;
        cmdt->cmd_colr = attr->colno;

        const sega3d_flags_t debug_flags =
            (SEGA3D_OBJECT_FLAGS_WIREFRAME | SEGA3D_OBJECT_FLAGS_NON_TEXTURED);

        /* For debugging */
        if ((object->flags & debug_flags) == SEGA3D_OBJECT_FLAGS_NONE) {
                const clip_flags_t or_clip_flags = (trans->polygon[0]->clip_flags |
                                                    trans->polygon[1]->clip_flags |
                                                    trans->polygon[2]->clip_flags |
                                                    trans->polygon[3]->clip_flags);
        
                if (or_clip_flags == CLIP_FLAGS_NONE) {
                        /* Since no clip flags are set, disable pre-clipping.
                         * This should help with performance */
                        cmdt->cmd_pmod |= VDP1_CMDT_PMOD_PRE_CLIPPING_DISABLE;
                }
        
                /* Even when there is not texture list, there is the default
                 * texture that zeroes out CMDSRCA and CMDSIZE */
                const TEXTURE * const textures = _internal_state->tlist->list;

                /* When there is no palette list, simply use the value directly */
                const PALETTE * const palettes = _internal_state->plist->list;

                if ((attr->dir == FUNC_Texture) && ((attr->sort & UseTexture) == UseTexture)) {
                        const TEXTURE * const texture = &textures[attr->texno];

                        if (palettes != NULL) {
                                const PALETTE * const palette = &palettes[attr->colno];

                                cmdt->cmd_colr = palette->Color;
                        } else {
                                cmdt->cmd_colr = attr->colno;
                        }

                        cmdt->cmd_srca = texture->CGadr;
                        cmdt->cmd_size = texture->HVsize;

                        cmdt->cmd_pmod |= VDP1_CMDT_PMOD_HSS_ENABLE;
                }
        } else {
                cmdt->cmd_ctrl = 0x0004 | ((object->flags & debug_flags) >> 1);
                cmdt->cmd_pmod = VDP1_CMDT_PMOD_END_CODE_DISABLE | VDP1_CMDT_PMOD_TRANS_PIXEL_DISABLE;
                cmdt->cmd_colr = 0xFFFF;
        }

        int16_vec2_t *cmd_vertex;
        cmd_vertex = (int16_vec2_t *)(&cmdt->cmd_xa);

        cmd_vertex->x = trans->polygon[0]->screen.x;
        cmd_vertex->y = trans->polygon[0]->screen.y;
        cmd_vertex++;

        cmd_vertex->x = trans->polygon[1]->screen.x;
        cmd_vertex->y = trans->polygon[1]->screen.y;
        cmd_vertex++;

        cmd_vertex->x = trans->polygon[2]->screen.x;
        cmd_vertex->y = trans->polygon[2]->screen.y;
        cmd_vertex++;

        cmd_vertex->x = trans->polygon[3]->screen.x;
        cmd_vertex->y = trans->polygon[3]->screen.y;
        cmd_vertex++;

        cmdt->cmd_grda = attr->gstb;

        if ((_internal_state->flags & FLAGS_FOG_ENABLED) != FLAGS_NONE) {
                if ((object->flags & SEGA3D_OBJECT_FLAGS_FOG_EXCLUDE) == SEGA3D_OBJECT_FLAGS_NONE) {
                        _fog_calculate(trans);
                }
        }
}

static void
_fog_calculate(const transform_t * const trans)
{
        vdp1_cmdt_t * const cmdt = trans->current_cmdt;

        if (trans->z_value < _internal_state->fog->start_z) {
                cmdt->cmd_colr = _internal_state->fog->near_ambient_color.raw;

                return;
        }

        if (trans->z_value >= _internal_state->fog->end_z) {
                cmdt->cmd_colr = _internal_state->fog->far_ambient_color.raw;

                return;
        }

        int32_t int_z_depth;
        int_z_depth = fix16_int16_muls(trans->z_value, _internal_state->fog->step);

        if (int_z_depth < 0) {
                int_z_depth = 0;
        }
        if (int_z_depth >= _internal_state->fog->depth_count) {
                int_z_depth = _internal_state->fog->depth_count - 1;
        }

        const int32_t depth_index __unused = _internal_state->fog->depth_z[int_z_depth];

        cmdt->cmd_colr = _internal_state->fog->depth_colors[depth_index].raw;
}

static bool
_screen_cull_test(const transform_t * const trans)
{
        const int16_vec2_t * const p0 = &trans->polygon[0]->screen;
        const int16_vec2_t * const p1 = &trans->polygon[1]->screen;
        const int16_vec2_t * const p2 = &trans->polygon[2]->screen;
        const int16_vec2_t * const p3 = &trans->polygon[3]->screen;

        const int32_vec2_t u1 = {
                .x = p1->x - p0->x,
                .y = p1->y - p0->y
        };

        const int32_vec2_t v1 = {
                .x = p3->x - p0->x,
                .y = p3->y - p0->y
        };

        /* Ideally, we only need to do a cross product on one winding order, but
         * in the case of triangles that combine two vertices, it's unknown
         * which two vertices are joined. The easiest solution is to test one
         * winding order, and if it fails, don't bother testing the other
         * winding order */

        const int32_t z1 = (u1.x * v1.y) - (u1.y * v1.x);

        if (z1 < 0) {
                return false;
        }

        const int16_vec2_t u2 = {
                .x = p3->x - p2->x,
                .y = p3->y - p2->y
        };

        const int16_vec2_t v2 = {
                .x = p1->x - p2->x,
                .y = p1->y - p2->y
        };

        const int16_t z2 = (u2.x * v2.y) - (u2.y * v2.x);

        return (z2 >= 0);
}

static bool
_object_sphere_cull_test(const transform_t * const trans)
{
        const fix16_plane_t * const clip_planes =
            (fix16_plane_t *)_internal_state->clip_planes;
        const sega3d_object_t * const object = trans->object;
        const sega3d_cull_sphere_t * const sphere = object->cull_shape;

        const FIXED * const world_matrix = (const FIXED *)sega3d_matrix_top();

        /* Transform the shape's origin
         * Because we're working with spheres and AABBs, it's enough to just
         * translate the center point. For AABBs, the user would be responsible
         * for updating the AABB */
        const fix16_vec3_t trans_origin = {
                .x = sphere->origin[X] + world_matrix[M03],
                .y = sphere->origin[Y] + world_matrix[M13],
                .z = sphere->origin[Z] + world_matrix[M23]
        };

        for (uint32_t i = 0; i < CLIP_PLANE_COUNT; i++) {
                const fix16_plane_t * const clip_plane = &clip_planes[i];

                fix16_vec3_t cp;
                fix16_vec3_sub(&trans_origin, &clip_plane->d, &cp);

                const fix16_t side = fix16_vec3_dot(&clip_plane->normal, &cp);

                /* Test for intersection */
                if (((side <= -sphere->radius) || (side >= sphere->radius)) && (side < FIX16(0.0f))) {
                        return true;
                }
        }

        return false;
}

static bool
_object_aabb_cull_test(const transform_t * const trans)
{
        const fix16_plane_t * const clip_planes =
            (fix16_plane_t *)_internal_state->clip_planes;
        const sega3d_object_t * const object = trans->object;
        const sega3d_cull_aabb_t * const aabb = object->cull_shape;

        const FIXED * const world_matrix = (const FIXED *)sega3d_matrix_top();

        const fix16_vec3_t aabb_min = {
                .x = world_matrix[M03] + aabb->origin[X] - aabb->length[X],
                .y = world_matrix[M13] + aabb->origin[Y] - aabb->length[Y],
                .z = world_matrix[M23] + aabb->origin[Z] - aabb->length[Z]
        };

        const fix16_vec3_t aabb_max = {
                .x = world_matrix[M03] + aabb->origin[X] + aabb->length[X],
                .y = world_matrix[M13] + aabb->origin[Y] + aabb->length[Y],
                .z = world_matrix[M23] + aabb->origin[Z] + aabb->length[Z]
        };

        /* Depending where the normal is pointing, find the point nearest to the
         * plane. Taking the dot product of the normal and this near point of
         * the AABB determines if the AABB is intersecting with the clip
         * plane */

        for (uint32_t i = 0; i < CLIP_PLANE_COUNT; i++) {
                const fix16_plane_t * const clip_plane = &clip_planes[i];

                fix16_vec3_t near_point;

                near_point.x = (clip_plane->normal.x < FIX16(0.0f)) ? aabb_min.x : aabb_max.x;
                near_point.y = (clip_plane->normal.y < FIX16(0.0f)) ? aabb_min.y : aabb_max.y;
                near_point.z = (clip_plane->normal.z < FIX16(0.0f)) ? aabb_min.z : aabb_max.z;

                fix16_vec3_t cp;
                fix16_vec3_sub(&near_point, &clip_plane->d, &cp);

                const fix16_t side = fix16_vec3_dot(&clip_plane->normal, &cp);

                if (side < FIX16(0.0f)) {
                        return true;
                }
        }

        return false;
}

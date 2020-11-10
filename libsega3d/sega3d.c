/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <sys/cdefs.h>

#include <vdp2/tvmd.h>

#include <cpu/cache.h>
#include <cpu/divu.h>

#include "sega3d.h"
#include "sega3d-internal.h"

#define DEPTH_FOG_START         FIX16(20.f)
#define DEPTH_FOG_END           FIX16(108.0f)
#define DEPTH_FOG_POW           (6)
#define DEPTH_FOG_COLOR_COUNT   (32)
#define DEPTH_FOG_COUNT         (64)

typedef enum {
        CLIP_FLAGS_NONE   = 0,
        CLIP_FLAGS_NEAR   = 1 << 0,
        CLIP_FLAGS_FAR    = 1 << 1,
        CLIP_FLAGS_LEFT   = 1 << 2,
        CLIP_FLAGS_RIGHT  = 1 << 3,
        CLIP_FLAGS_TOP    = 1 << 4,
        CLIP_FLAGS_BOTTOM = 1 << 5,
} clip_flags_t;

typedef enum {
        FLAGS_NONE        = 0,
        FLAGS_INITIALIZED = 1 << 0,
        FLAGS_FOG_ENABLED = 1 << 1,
} flags_t;

typedef struct {
        FIXED point[XYZ];
        int16_t screen[XY];
        clip_flags_t clip_flags;
} transform_proj_t;

typedef struct {
        const FIXED *dst_matrix;

        uint16_t index;
        FIXED z_center;
        const VECTOR *polygon_normal;
        transform_proj_t *polygon[4] __aligned(16);

        transform_proj_t projs[VERTEX_POOL_SIZE] __aligned(16);
} transform_t;

static_assert(sizeof(transform_proj_t) == 20);

extern void _internal_tlist_init(void);
extern void _internal_matrix_init(void);

extern void _internal_sort_clear(void);
extern void _internal_sort_add(void *packet, int32_t pz);
extern void _internal_sort_iterate(iterate_fn fn);

static const color_rgb1555_t _depth_fog_colors[DEPTH_FOG_COLOR_COUNT] = {
        COLOR_RGB1555(1,  0, 16, 16),
        COLOR_RGB1555(1,  1, 16, 16),
        COLOR_RGB1555(1,  2, 16, 16),
        COLOR_RGB1555(1,  3, 16, 16),
        COLOR_RGB1555(1,  4, 16, 16),
        COLOR_RGB1555(1,  5, 16, 16),
        COLOR_RGB1555(1,  6, 16, 16),
        COLOR_RGB1555(1,  7, 16, 16),
        COLOR_RGB1555(1,  8, 16, 16),
        COLOR_RGB1555(1,  9, 16, 16),
        COLOR_RGB1555(1, 10, 16, 16),
        COLOR_RGB1555(1, 11, 16, 16),
        COLOR_RGB1555(1, 12, 16, 16),
        COLOR_RGB1555(1, 13, 16, 16),
        COLOR_RGB1555(1, 14, 16, 16),
        COLOR_RGB1555(1, 15, 16, 16),
        COLOR_RGB1555(1, 16, 16, 16),
        COLOR_RGB1555(1, 17, 16, 16),
        COLOR_RGB1555(1, 18, 16, 16),
        COLOR_RGB1555(1, 19, 16, 16),
        COLOR_RGB1555(1, 20, 16, 16),
        COLOR_RGB1555(1, 21, 16, 16),
        COLOR_RGB1555(1, 22, 16, 16),
        COLOR_RGB1555(1, 23, 16, 16),
        COLOR_RGB1555(1, 24, 16, 16),
        COLOR_RGB1555(1, 25, 16, 16),
        COLOR_RGB1555(1, 26, 16, 16),
        COLOR_RGB1555(1, 27, 16, 16),
        COLOR_RGB1555(1, 28, 16, 16),
        COLOR_RGB1555(1, 29, 16, 16),
        COLOR_RGB1555(1, 30, 16, 16),
        COLOR_RGB1555(1, 31, 16, 16)
};

static const uint8_t _depth_fog_z[DEPTH_FOG_COUNT] = {
         0,  0,  0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,
         7,  8,  9, 10, 11, 11, 12, 13, 14, 15, 16, 17, 17, 18, 19, 20,
        21, 21, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28,
        28, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31
};

static struct {
        flags_t flags;

        /* Current object */
        sega3d_object_t *object;
        /* Current sorting orderlist */
        vdp1_cmdt_orderlist_t *sort_orderlist;
        FIXED cached_inv_top;
        FIXED cached_inv_right;
        int16_t cached_sw_2;
        int16_t cached_sh_2;

        sega3d_fog_t fog;
        FIXED fog_start_z;
        FIXED fog_end_z;
        uint16_t fog_len;

        sega3d_info_t view;

        vdp1_cmdt_t cmdt_end;
} _state __aligned(4);

static void _sort_iterate(sort_single_t *single);

static void _cmdt_prepare(const transform_t *trans, vdp1_cmdt_t *cmdt);
static void _fog_calculate(const transform_t *trans, vdp1_cmdt_t *cmdt);
static bool _view_cull_test(const transform_t * const trans, const transform_proj_t * const trans_proj);
static bool _screen_cull_test(const transform_t *trans) __unused;

static inline FIXED __always_inline __used
_vertex_transform(const FIXED *p, const FIXED *matrix)
{
        register int32_t tmp1;
        register int32_t tmp2;
        register int32_t pt;

        __asm__ volatile ("\tmov %[matrix], %[tmp1]\n"
                          "\tmov %[p], %[pt]\n"
                          "\tclrmac\n"
                          "\tmac.l @%[pt]+, @%[tmp1]+\n"
                          "\tmac.l @%[pt]+, @%[tmp1]+\n"
                          "\tmac.l @%[pt]+, @%[tmp1]+\n"
                          "\tmov.l @%[tmp1], %[pt]\n"
                          "\tsts mach, %[tmp1]\n"
                          "\tsts macl, %[tmp2]\n"
                          "\txtrct %[tmp1], %[tmp2]\n"
                          "add %[tmp2], %[pt]\n"
            /* Output */
            : [tmp1] "=&r" (tmp1),
              [tmp2] "=&r" (tmp2),
              [pt]   "=&r" (pt)
            /* Input */
            : [p]      "r" (p),
              [matrix] "r" (matrix)
            : "mach", "macl", "memory");

        return pt;
}

void
sega3d_init(void)
{
        const sega3d_fog_t default_fog = {
                .depth_colors = _depth_fog_colors,
                .depth_z = _depth_fog_z,
                .pow = DEPTH_FOG_POW,
                .gouraud_idx = 0
        };

        /* Prevent re-initialization */
        if ((_state.flags & FLAGS_INITIALIZED) != FLAGS_NONE) {
                return;
        }

        _state.flags = FLAGS_INITIALIZED;

        sega3d_perspective_set(DEGtoANG(90.0f));

        sega3d_fog_set(&default_fog);
        sega3d_fog_limits_set(DEPTH_FOG_START, DEPTH_FOG_END);

        vdp1_cmdt_end_set(&_state.cmdt_end);

        _internal_tlist_init();
        _internal_matrix_init();
}

void
sega3d_perspective_set(ANGLE fov)
{
#define AW_2 FIX16(12.446f) /* ([film-aperature-width = 0.980] * inch->mm) / 2) */
#define AH_2 FIX16(9.3345f) /* ([film-aperature-height = 0.735] * inch->mm) / 2) */

        const FIXED fov_angle = fix16_mul(fov, FIX16_2PI) >> 1;

        uint16_t i_width;
        uint16_t i_height;

        vdp2_tvmd_display_res_get(&i_width, &i_height);

        /* Let's maintain a 4:3 ratio */
        if (i_height < 240) {
                i_height = 240;
        }

        const FIXED screen_width = fix16_int32_from(i_width);
        const FIXED screen_height = fix16_int32_from(i_height);

        _state.cached_sw_2 = i_width / 2;
        _state.cached_sh_2 = i_height / 2;

        cpu_divu_fix16_set(screen_width, screen_height);
        _state.view.ratio = cpu_divu_quotient_get();

        cpu_divu_fix16_set(FIX16(1.0f), fix16_tan(fov_angle));
        _state.view.focal_length = fix16_mul(AW_2, cpu_divu_quotient_get());

        _state.view.view[X] = 0;
        _state.view.view[Y] = 0;
        _state.view.view[Z] = -_state.view.focal_length;

        _state.view.near = _state.view.focal_length;

        cpu_divu_fix16_set(AH_2, _state.view.focal_length);
        _state.view.top = cpu_divu_quotient_get();
        _state.view.right = -fix16_mul(-_state.view.top, _state.view.ratio);
        _state.view.left = -_state.view.right;
        _state.view.bottom = -_state.view.top;

        cpu_divu_fix16_set(screen_height, _state.view.top << 1);
        _state.cached_inv_top = cpu_divu_quotient_get();

        cpu_divu_fix16_set(screen_width, _state.view.right << 1);
        _state.cached_inv_right = cpu_divu_quotient_get();
}

void
sega3d_info_get(sega3d_info_t *info)
{
        (void)memcpy(info, &_state.view, sizeof(sega3d_info_t));
}

Uint16
sega3d_object_polycount_get(const sega3d_object_t *object)
{
        const PDATA * const pdata = object->pdata;

        return pdata->nbPolygon;
}

void
sega3d_fog_set(const sega3d_fog_t *fog)
{
        _state.flags &= ~FLAGS_FOG_ENABLED;

        if (fog == NULL) {
                (void)memset(&_state.fog, 0, sizeof(sega3d_fog_t));

                return;
        }

        assert(fog->depth_colors != NULL);
        assert(fog->depth_z != NULL);

        _state.flags |= FLAGS_FOG_ENABLED;

        (void)memcpy(&_state.fog, fog, sizeof(sega3d_fog_t));

        _state.fog_len = 1 << fog->pow;
}

void
sega3d_fog_limits_set(FIXED start_z, FIXED end_z)
{
        _state.fog_start_z = start_z;
        _state.fog_end_z = end_z;
}

static void
_vertex_pool_transform(transform_t *trans, POINT const * points, uint32_t vertex_count)
{
        const FIXED *current_point = (const FIXED *)points; 

        transform_proj_t *trans_proj;
        trans_proj = &trans->projs[0];

        for (uint32_t index = 0; index < vertex_count; index++) {
                trans_proj->point[Z] = _vertex_transform(current_point, &trans->dst_matrix[8]);
                trans_proj->clip_flags = CLIP_FLAGS_NONE;

                /* In case the projected Z value is on or behind the near plane */
                if (trans_proj->point[Z] <= _state.view.near) {
                        trans_proj->clip_flags |= CLIP_FLAGS_NEAR;

                        trans_proj->point[Z] = _state.view.near;
                }

                cpu_divu_fix16_set(_state.cached_inv_right, trans_proj->point[Z]);
        
                trans_proj->point[X] = _vertex_transform(current_point, &trans->dst_matrix[0]);
                trans_proj->screen[X] = fix16_int16_muls(trans_proj->point[X], cpu_divu_quotient_get());

                cpu_divu_fix16_set(_state.cached_inv_top, trans_proj->point[Z]);

                if (trans_proj->screen[X] < -_state.cached_sw_2) {
                        trans_proj->clip_flags |= CLIP_FLAGS_LEFT;
                } else if (trans_proj->screen[X] > _state.cached_sw_2) {
                        trans_proj->clip_flags |= CLIP_FLAGS_RIGHT;
                }

                trans_proj->point[Y] = _vertex_transform(current_point, &trans->dst_matrix[4]);
                trans_proj->screen[Y] = fix16_int16_muls(trans_proj->point[Y], cpu_divu_quotient_get());

                if (trans_proj->screen[Y] > _state.cached_sh_2) {
                        trans_proj->clip_flags |= CLIP_FLAGS_TOP;
                } else if (trans_proj->screen[Y] < -_state.cached_sh_2) {
                        trans_proj->clip_flags |= CLIP_FLAGS_BOTTOM;
                }

                trans_proj++;
                current_point += 3;
        }
}

static void
_polygon_process(const sega3d_object_t *object, transform_t *trans,
    POLYGON const *polygons, uint32_t polygon_count)
{
        vdp1_cmdt_t *transform_cmdt;
        transform_cmdt = &object->cmdts[object->offset];
        
        for (trans->index = 0; trans->index < polygon_count; trans->index++,  polygons++) {
                const uint16_t * vertices = &polygons->Vertices[0];

                trans->polygon_normal = &polygons->norm;

                trans->polygon[0] = &trans->projs[*(vertices++)];
                trans->polygon[1] = &trans->projs[*(vertices++)];
                trans->polygon[2] = &trans->projs[*(vertices++)];
                trans->polygon[3] = &trans->projs[*vertices];

                if ((object->flags & SEGA3D_OBJECT_FLAGS_CULL_VIEW) != SEGA3D_OBJECT_FLAGS_NONE) {
                        if ((_view_cull_test(trans, trans->polygon[0]))) {
                                continue;
                        }
                }

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

                const FIXED z_avg = trans->polygon[0]->point[Z] +
                                    trans->polygon[1]->point[Z] +
                                    trans->polygon[2]->point[Z] +
                                    trans->polygon[3]->point[Z];

                /* Divide by 4 to get the average (bit shift) */
                trans->z_center = z_avg >> 2;

                _cmdt_prepare(trans, transform_cmdt);

                _internal_sort_add(transform_cmdt, fix16_int32_to(trans->z_center));

                transform_cmdt++;
        }
}

void
sega3d_object_transform(sega3d_object_t *object)
{
        static transform_t _transform __aligned(16) __unused;

        _state.object = object;

        _internal_sort_clear();

        const PDATA * const pdata = object->pdata;

        const uint32_t polygon_count =
            (pdata->nbPolygon < (PACKET_SIZE - 1)) ? pdata->nbPolygon : (PACKET_SIZE - 1);
        const uint32_t vertex_count =
            (pdata->nbPoint < VERTEX_POOL_SIZE) ? pdata->nbPoint : VERTEX_POOL_SIZE;

        transform_t *trans;
        trans = &_transform;

        trans->dst_matrix = (const FIXED *)sega3d_matrix_top();

        _vertex_pool_transform(trans, pdata->pntbl, vertex_count);
        _polygon_process(object, trans, pdata->pltbl, polygon_count);

        _state.sort_orderlist = &object->cmdt_orderlist[object->offset];

        _internal_sort_iterate(_sort_iterate);

        /* Update count */
        object->count = _state.sort_orderlist - object->cmdt_orderlist;

        /* Fetch the last command table pointer before setting the indirect mode
         * transfer end bit */
        _state.sort_orderlist->cmdt = &_state.cmdt_end;

        vdp1_cmdt_orderlist_end(_state.sort_orderlist);
}

static void
_sort_iterate(sort_single_t *single)
{
        /* No need to clear the end bit, as setting the "source" clobbers the
         * bit */
        _state.sort_orderlist->cmdt = single->packet;

        _state.sort_orderlist++;
}

static void
_cmdt_prepare(const transform_t *trans, vdp1_cmdt_t *cmdt)
{
        const PDATA * const pdata = _state.object->pdata;

        const ATTR *attr;
        attr = &pdata->attbl[trans->index];

        cmdt->cmd_ctrl = attr->dir; /* We care about (Dir) and (Comm) bits */
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = attr->atrb;
        cmdt->cmd_colr = attr->colno;

        /* For debugging */
        if ((_state.object->flags & SEGA3D_OBJECT_FLAGS_WIREFRAME) == SEGA3D_OBJECT_FLAGS_WIREFRAME) {
                cmdt->cmd_ctrl = 0x0005;
                cmdt->cmd_pmod = VDP1_CMDT_PMOD_END_CODE_DISABLE | VDP1_CMDT_PMOD_TRANS_PIXEL_DISABLE;
                cmdt->cmd_colr = 0xFFFF;
        }
        const clip_flags_t or_clip_flags = (trans->polygon[0]->clip_flags |
                                            trans->polygon[1]->clip_flags |
                                            trans->polygon[2]->clip_flags |
                                            trans->polygon[3]->clip_flags);

        if (or_clip_flags == CLIP_FLAGS_NONE) {
                /* Since no clip flags are set, disable pre-clipping. This
                 * should help with performance */
                cmdt->cmd_pmod |= VDP1_CMDT_PMOD_PRE_CLIPPING_DISABLE;
        }

        /* Even when there is not texture list, there is the default
         * texture that zeroes out CMDSRCA and CMDSIZE */
        const TEXTURE *texture;
        texture = sega3d_tlist_tex_get(attr->texno);

#ifdef DEBUG
        const uint16_t tlist_count = sega3d_tlist_count_get();

        /* If the texture number is zero, it could imply no texture.
         * Even if the texture list is empty, it's considered valid */
        if ((tlist_count > 0) && (attr->texno != 0)) {
                assert(attr->texno < tlist_count);
        }
#endif /* DEBUG */

        cmdt->cmd_srca = texture->CGadr;
        cmdt->cmd_size = texture->HVsize;

        int16_vec2_t *cmd_vertex;
        cmd_vertex = (int16_vec2_t *)(&cmdt->cmd_xa);

        cmd_vertex->x = trans->polygon[0]->screen[X];
        cmd_vertex->y = trans->polygon[0]->screen[Y];
        cmd_vertex++;

        cmd_vertex->x = trans->polygon[1]->screen[X];
        cmd_vertex->y = trans->polygon[1]->screen[Y];
        cmd_vertex++;

        cmd_vertex->x = trans->polygon[2]->screen[X];
        cmd_vertex->y = trans->polygon[2]->screen[Y];
        cmd_vertex++;

        cmd_vertex->x = trans->polygon[3]->screen[X];
        cmd_vertex->y = trans->polygon[3]->screen[Y];
        cmd_vertex++;

        cmdt->cmd_grda = attr->gstb;

        if ((_state.flags & FLAGS_FOG_ENABLED) != FLAGS_NONE) {
                _fog_calculate(trans, cmdt);
        }
}

static void
_fog_calculate(const transform_t *trans, vdp1_cmdt_t *cmdt)
{
        if ((trans->z_center < _state.fog_start_z) ||
            (trans->z_center >= _state.fog_end_z)) {
                return;
        }

        int32_t int_z_depth;
        int_z_depth = fix16_int32_to(trans->z_center);

        if (int_z_depth < 0) {
                int_z_depth = 0;
        }
        if (int_z_depth >= _state.fog_len) {
                int_z_depth = _state.fog_len - 1;
        }

        const int32_t depth_index = _depth_fog_z[int_z_depth];

        cmdt->cmd_colr = _depth_fog_colors[depth_index].raw;
}

static bool
_screen_cull_test(const transform_t *trans __unused)
{
        return false;
        /* const FIXED * const proj_ptr = &trans->proj_screen[0]; */
        /* const FIXED * const next_proj_ptr = &trans->proj_screen[(1 * XY)]; */

        /* const FIXED cull_z = fix16_mul(proj_ptr[X], next_proj_ptr[Y]) - */
        /*     fix16_mul(proj_ptr[Y], next_proj_ptr[X]); */

        /* return (cull_z < 0); */
}

static bool
_view_cull_test(const transform_t * const trans, const transform_proj_t * const trans_proj)
{
        const FIXED * const cull_p = (const FIXED *)&trans_proj->point[0];

        FIXED view_p[XYZ];

        /* XXX: Compute this outside!!! */
        if ((_state.object->flags & SEGA3D_OBJECT_FLAGS_CULL_VIEW_ROT) != SEGA3D_OBJECT_FLAGS_NONE) {
                /* Invert 3x3 rotation matrix */
        } else {
                view_p[X] = -trans->dst_matrix[3];
                view_p[Y] = -trans->dst_matrix[7];
                view_p[Z] = -trans->dst_matrix[11];
        }

        const fix16_t cull_point = fix16_vec3_inline_dot((const fix16_vec3_t *)cull_p,
            (const fix16_vec3_t *)trans->polygon_normal);

        const fix16_t cull_view = fix16_vec3_inline_dot((const fix16_vec3_t *)view_p,
            (const fix16_vec3_t *)trans->polygon_normal);

        return (cull_view < cull_point);
}

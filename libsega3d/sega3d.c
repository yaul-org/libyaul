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
} __packed flags_t;

typedef struct {
        FIXED dst_matrix[MTRX];
        FIXED proj_xy[XY];
        clip_flags_t clip_flags[4];
        FIXED z_center;
        FIXED z_avg;
        FIXED proj_z[4];
        FIXED proj_screen[4 * XY];
        uint16_t index;

        FIXED scale_inv_top;
        FIXED scale_inv_right;
} transform_t;

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

        sega3d_info_t view;
        FIXED cached_inv_top;
        FIXED cached_inv_right;
        int16_t cached_sw_2;
        int16_t cached_sh_2;

        vdp1_cmdt_t cmdt_end;

        sega3d_fog_t fog;
        FIXED fog_start_z;
        FIXED fog_end_z;
        uint16_t fog_len;

        /* Current object */
        sega3d_object_t *object;
        /* Set iteration function */
        sega3d_iterate_fn iterate_fn;
} _state;

static void _sort_iterate(sort_single_t *single);

static void _cmdt_prepare(const transform_t *trans, vdp1_cmdt_t *cmdt);
static void _fog_calculate(const transform_t *trans, vdp1_cmdt_t *cmdt);
static bool _view_cull_test(const transform_t *trans);
static bool _screen_cull_test(const transform_t *trans);

static inline FIXED __always_inline __used
_vertex_transform(const FIXED *p, const FIXED *matrix)
{
        register int32_t tmp1;
        register int32_t tmp2;
        register int32_t pt;

        __asm__ volatile (
                          "\tmov %[matrix], %[tmp1]\n"
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

void
sega3d_object_transform(sega3d_object_t *object)
{
        static transform_t _transform __aligned(16) __unused;

        _state.object = object;

        _internal_sort_clear();

        const PDATA * const pdata = object->pdata;
        const POINT * const points = pdata->pntbl;

        const uint32_t polygon_count =
            (pdata->nbPolygon < (PACKET_SIZE - 1)) ? pdata->nbPolygon : (PACKET_SIZE - 1);

        /* cpu_cache_purge(); */
        /* cpu_cache_way_mode_set(CPU_CACHE_MODE_2_WAY | CPU_CACHE_MODE_OD_DIS); */

        transform_t *trans;
        /* trans = (transform_t *)CPU_CACHE_DATA_RW; */
        trans = &_transform;

        /* (void)memcpy(trans->point_pool, points, sizeof(POINT) * 128); */

        vdp1_cmdt_t *transform_cmdts;
        transform_cmdts = _state.object->cmdts;

        const FIXED * const src_matrix = (const FIXED *)sega3d_matrix_top();

        trans->dst_matrix[ 0] = src_matrix[M00];
        trans->dst_matrix[ 1] = src_matrix[M01];
        trans->dst_matrix[ 2] = src_matrix[M02];
        trans->dst_matrix[ 3] = src_matrix[M30];

        trans->dst_matrix[ 4] = src_matrix[M10];
        trans->dst_matrix[ 5] = src_matrix[M11];
        trans->dst_matrix[ 6] = src_matrix[M12];
        trans->dst_matrix[ 7] = src_matrix[M31];

        trans->dst_matrix[ 8] = src_matrix[M20];
        trans->dst_matrix[ 9] = src_matrix[M21];
        trans->dst_matrix[10] = src_matrix[M22];
        trans->dst_matrix[11] = src_matrix[M32];

        for (uint32_t index = 0; index < polygon_count; index++) {
                const POLYGON * const polygon = &pdata->pltbl[index];

                trans->index = index;
                trans->z_avg = 0;

                if ((object->flags & SEGA3D_OBJECT_FLAGS_CULL_VIEW) != SEGA3D_OBJECT_FLAGS_NONE) {
                        bool cull = _view_cull_test(trans);

                        if (cull) {
                                continue;
                        }
                }

                for (uint32_t v = 0; v < 4; v++) {
                        const int16_t vertex = polygon->Vertices[v];
                        const FIXED * const point = (const FIXED *)&points[vertex];
                        /* const FIXED * const point = (const FIXED *)&trans->point_pool[vertex & 127]; */

                        trans->proj_z[v] = _vertex_transform(point, &trans->dst_matrix[8]);

                        trans->clip_flags[v] = CLIP_FLAGS_NONE;

                        /* In case the projected Z value is on or behind the near plane */
                        if (trans->proj_z[v] <= _state.view.near) {
                                trans->clip_flags[v] |= CLIP_FLAGS_NEAR;

                                trans->proj_z[v] = _state.view.near;
                        }

                        cpu_divu_fix16_set(_state.cached_inv_right, trans->proj_z[v]);

                        trans->z_avg += trans->proj_z[v];

                        trans->proj_xy[X] = _vertex_transform(point, &trans->dst_matrix[0]);
                        trans->scale_inv_right = cpu_divu_quotient_get();
                        trans->proj_xy[X] = fix16_mul(trans->proj_xy[X], trans->scale_inv_right);

                        cpu_divu_fix16_set(_state.cached_inv_top, trans->proj_z[v]);

                        FIXED *proj_ptr;
                        proj_ptr = &trans->proj_screen[v * XY];

                        proj_ptr[X] = fix16_int32_to(trans->proj_xy[X]);

                        if (proj_ptr[X] < -_state.cached_sw_2) {
                                trans->clip_flags[v] |= CLIP_FLAGS_LEFT;
                        }
                        if (proj_ptr[X] > _state.cached_sw_2) {
                                trans->clip_flags[v] |= CLIP_FLAGS_RIGHT;
                        }

                        trans->proj_xy[Y] = _vertex_transform(point, &trans->dst_matrix[4]);
                        trans->scale_inv_top = cpu_divu_quotient_get();

                        trans->proj_xy[Y] = fix16_mul(trans->proj_xy[Y], trans->scale_inv_top);
                        proj_ptr[Y] = fix16_int32_to(trans->proj_xy[Y]);

                        if (proj_ptr[Y] > _state.cached_sh_2) {
                                trans->clip_flags[v] |= CLIP_FLAGS_TOP;
                        }
                        if (proj_ptr[Y] < -_state.cached_sh_2) {
                                trans->clip_flags[v] |= CLIP_FLAGS_BOTTOM;
                        }
                }

                const clip_flags_t and_clip_flags = (trans->clip_flags[0] &
                                                     trans->clip_flags[1] &
                                                     trans->clip_flags[2] &
                                                     trans->clip_flags[3]);

                if (and_clip_flags != CLIP_FLAGS_NONE) {
                        continue;
                }

                if ((object->flags & SEGA3D_OBJECT_FLAGS_CULL_SCREEN) != SEGA3D_OBJECT_FLAGS_NONE) {
                        bool cull = _screen_cull_test(trans);

                        if (cull) {
                                continue;
                        }
                }

                /* Divide by 4 to get the average (bit shift) */
                trans->z_center = trans->z_avg >> 2;

                vdp1_cmdt_t *transform_cmdt;
                transform_cmdt = &transform_cmdts[_state.object->offset + index];

                _cmdt_prepare(trans, transform_cmdt);

                _internal_sort_add(transform_cmdt, fix16_int32_to(trans->z_center));
        }

        _state.object->count = 0;

        _internal_sort_iterate(_sort_iterate);

        /* Take the the last entry in the orderlist and terminate, as well as
         * the command table */
        vdp1_cmdt_orderlist_t *cmdt_orderlist;
        cmdt_orderlist = &_state.object->cmdt_orderlist[_state.object->offset + _state.object->count];

        vdp1_cmdt_end_set(&_state.cmdt_end);

        /* Fetch the last command table pointer before setting the indirect mode
         * transfer end bit */
        cmdt_orderlist->control.end = false;
        cmdt_orderlist->cmdt = &_state.cmdt_end;

        cmdt_orderlist->control.end = true;

        /* cpu_cache_way_mode_set(CPU_CACHE_MODE_4_WAY); */
        /* cpu_cache_enable(); */
}

static void
_sort_iterate(sort_single_t *single)
{
        vdp1_cmdt_orderlist_t *cmdt_orderlist;
        cmdt_orderlist = &_state.object->cmdt_orderlist[_state.object->offset + _state.object->count];

        cmdt_orderlist->cmdt = single->packet;
        cmdt_orderlist->control.end = false;

        _state.object->count++;
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
                cmdt->cmd_ctrl = 0x0004;
                cmdt->cmd_pmod = VDP1_CMDT_PMOD_END_CODE_DISABLE | VDP1_CMDT_PMOD_TRANS_PIXEL_DISABLE;
                cmdt->cmd_colr = 0xFFFF;
        }

        const clip_flags_t or_clip_flags = (trans->clip_flags[0] |
                                            trans->clip_flags[1] |
                                            trans->clip_flags[2] |
                                            trans->clip_flags[3]);

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

        for (uint32_t v = 0; v < 4; v++) {
                const FIXED * const proj_ptr = &trans->proj_screen[v * XY];

                cmd_vertex[v].x = proj_ptr[X];
                cmd_vertex[v].y = proj_ptr[Y];
        }

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
_screen_cull_test(const transform_t *trans)
{
        const FIXED * const proj_ptr = &trans->proj_screen[0];
        const FIXED * const next_proj_ptr = &trans->proj_screen[(1 * XY)];

        const FIXED cull_z = fix16_mul(proj_ptr[X], next_proj_ptr[Y]) -
            fix16_mul(proj_ptr[Y], next_proj_ptr[X]);

        return (cull_z < 0);
}

static bool
_view_cull_test(const transform_t *trans)
{
        const PDATA * const pdata = _state.object->pdata;
        const POLYGON * const polygon = &pdata->pltbl[trans->index];
        const POINT * const points = pdata->pntbl;

        const int16_t cull_vi = polygon->Vertices[0];

        const FIXED * const cull_p = (const FIXED *)&points[cull_vi];

        FIXED view_p[XYZ];

        if ((_state.object->flags & SEGA3D_OBJECT_FLAGS_CULL_VIEW_ROT) != SEGA3D_OBJECT_FLAGS_NONE) {
                /* Invert 3x3 rotation matrix */
        } else {
                view_p[X] = -trans->dst_matrix[3];
                view_p[Y] = -trans->dst_matrix[7];
                view_p[Z] = -trans->dst_matrix[11];
        }

        const fix16_t cull_point = fix16_vec3_dot((const fix16_vec3_t *)cull_p,
            (const fix16_vec3_t *)&polygon->norm);

        const fix16_t cull_view = fix16_vec3_dot((const fix16_vec3_t *)view_p,
            (const fix16_vec3_t *)&polygon->norm);

        return (cull_view < cull_point);
}

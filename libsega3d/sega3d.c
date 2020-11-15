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

#include <vdp.h>
#include <vdp2/tvmd.h>

#include <cpu/cache.h>
#include <cpu/divu.h>
#include <cpu/frt.h>

#include "sega3d.h"
#include "sega3d-internal.h"

#define DEPTH_FOG_START         FIX16(1.f)
#define DEPTH_FOG_END           FIX16(512.0f)
#define DEPTH_FOG_POW           (6)
#define DEPTH_FOG_STEP          FIX16(1 / 8.0f)
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
        CLIP_FLAGS_SIDE   = 1 << 6,
} clip_flags_t;

typedef enum {
        FLAGS_NONE        = 0,
        FLAGS_INITIALIZED = 1 << 0,
        FLAGS_FOG_ENABLED = 1 << 1,
} flags_t;

typedef struct {
        FIXED point_z;
        int16_vec2_t screen;
        clip_flags_t clip_flags;
} __aligned(16) transform_proj_t;

typedef struct {
        /* Z center of the polygon */
        FIXED z_center;
        /* Pointers to the pool that make up the polygon */
        transform_proj_t *polygon[4];
        /* Current matrix */
        const FIXED *dst_matrix;
        /* Polygon index */
        uint16_t index;

        /* Pool containing the projected data */
        transform_proj_t proj_pool[VERTEX_POOL_SIZE] __aligned(16);
} __aligned(16) transform_t;

static_assert(sizeof(transform_proj_t) == 16);

extern void _internal_tlist_init(void);
extern void _internal_matrix_init(void);

extern void _internal_sort_init(void);
extern void _internal_sort_clear(void);
extern void _internal_sort_add(void *packet, int32_t pz);
extern void _internal_sort_iterate(iterate_fn fn);

extern void _internal_asm_vertex_pool_transform(void *state, transform_t *trans, POINT const * points, uint32_t vertex_count);

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

        /* Current processing orderlist */
        VECTOR cached_view;
        FIXED cached_inv_top;
        FIXED cached_inv_right;
        sega3d_info_t info;
        int16_t cached_sw_2;
        int16_t cached_sh_2;

        /* Start/finish state */
        vdp1_cmdt_orderlist_t *current_orderlist;
        vdp1_cmdt_orderlist_t *orderlist;
        vdp1_cmdt_t *current_cmdt;

        sega3d_fog_t fog;
        FIXED fog_start_z;
        FIXED fog_end_z;
        uint16_t fog_len;
} _state __aligned(4);

static vdp1_cmdt_t _cmdt_end;

static void _sort_iterate(sort_single_t *single);
static void _vertex_pool_transform(transform_t *trans, POINT const * points, uint32_t vertex_count);
static void _vertex_pool_clipping(transform_t *trans, uint32_t vertex_count);
static void _polygon_process(const sega3d_object_t *object, transform_t *trans,
    POLYGON const *polygons, uint32_t polygon_count);
static void _cmdt_prepare(const sega3d_object_t *object, const transform_t *trans,
    vdp1_cmdt_t *cmdt);
static void _cmdt_prepare(const sega3d_object_t *object, const transform_t *trans,
    vdp1_cmdt_t *cmdt);
static void _fog_calculate(const transform_t *trans, vdp1_cmdt_t *cmdt) __hot;
static bool _screen_cull_test(const transform_t *trans) __hot;

static inline FIXED __always_inline __used
_vertex_transform(const FIXED * __restrict p, const FIXED * __restrict matrix)
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
                .step = DEPTH_FOG_STEP,
                .near_ambient_color = _depth_fog_colors[0],
                .far_ambient_color = _depth_fog_colors[31],
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

        vdp1_cmdt_end_set(&_cmdt_end);

        _internal_tlist_init();
        _internal_matrix_init();

        _internal_sort_init();
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
        _state.info.ratio = cpu_divu_quotient_get();

        cpu_divu_fix16_set(FIX16(1.0f), fix16_tan(fov_angle));
        _state.info.focal_length = fix16_mul(AW_2, cpu_divu_quotient_get());

        _state.info.near = _state.info.focal_length;

        cpu_divu_fix16_set(AH_2, _state.info.focal_length);
        _state.info.top = cpu_divu_quotient_get();
        _state.info.right = -fix16_mul(-_state.info.top, _state.info.ratio);
        _state.info.left = -_state.info.right;
        _state.info.bottom = -_state.info.top;

        cpu_divu_fix16_set(screen_height, _state.info.top << 1);
        _state.cached_inv_top = cpu_divu_quotient_get();

        cpu_divu_fix16_set(screen_width, _state.info.right << 1);
        _state.cached_inv_right = cpu_divu_quotient_get();
}

void
sega3d_info_get(sega3d_info_t *info)
{
        (void)memcpy(info, &_state.info, sizeof(sega3d_info_t));
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

Uint16
sega3d_object_polycount_get(const sega3d_object_t *object)
{
        const PDATA * const pdata = object->pdata;

        return pdata->nbPolygon;
}

void
sega3d_start(vdp1_cmdt_orderlist_t *orderlist, uint16_t orderlist_offset, vdp1_cmdt_t *cmdts)
{
        assert(orderlist != NULL);

        _internal_sort_clear();

        _state.orderlist = orderlist;
        _state.current_orderlist = &orderlist[orderlist_offset];
        _state.current_cmdt = cmdts;
}

void
sega3d_finish(sega3d_results_t *results)
{
        _internal_sort_iterate(_sort_iterate);

        /* Fetch the last command table pointer before setting the indirect mode
         * transfer end bit */
        _state.current_orderlist->cmdt = &_cmdt_end;

        vdp1_cmdt_orderlist_end(_state.current_orderlist);

        vdp1_sync_cmdt_orderlist_put(_state.orderlist, NULL, NULL);

        if (results != NULL) {
                results->count = _state.current_orderlist - _state.orderlist;
        }
}

void
sega3d_object_transform(const sega3d_object_t *object)
{
        static transform_t transform __aligned(16);

        const PDATA * const pdata = object->pdata;

        const uint32_t polygon_count =
            (pdata->nbPolygon < (PACKET_SIZE - 1)) ? pdata->nbPolygon : (PACKET_SIZE - 1);
        const uint32_t vertex_count =
            (pdata->nbPoint < VERTEX_POOL_SIZE) ? pdata->nbPoint : VERTEX_POOL_SIZE;

        if ((vertex_count == 0) || (polygon_count == 0)) {
                return;
        }

        transform_t *trans;
        trans = &transform;

        trans->dst_matrix = (const FIXED *)sega3d_matrix_top();

        /* _internal_asm_vertex_pool_transform(&_state, trans, pdata->pntbl, vertex_count); */
        _vertex_pool_transform(trans, pdata->pntbl, vertex_count);
        _vertex_pool_clipping(trans, vertex_count);
        _polygon_process(object, trans, pdata->pltbl, polygon_count);
}

static __noinline __used void
_vertex_pool_transform(transform_t *trans, POINT const * points, uint32_t vertex_count)
{
        const FIXED *current_point = (const FIXED *)points;
        const FIXED *last_point = (const FIXED *)points[vertex_count];

        transform_proj_t *trans_proj;
        trans_proj = &trans->proj_pool[0];

        const FIXED inv_right = _state.cached_inv_right;
        const FIXED z_near = _state.info.near;

        do {
                trans_proj->clip_flags = CLIP_FLAGS_NONE; 

                trans_proj->point_z = _vertex_transform(current_point, &trans->dst_matrix[M20]);

                /* In case the projected Z value is on or behind the near plane */
                if (trans_proj->point_z <= z_near) {
                        trans_proj->clip_flags |= CLIP_FLAGS_NEAR;

                        trans_proj->point_z = z_near;
                }

                cpu_divu_fix16_set(inv_right, trans_proj->point_z);

                const FIXED point_x = _vertex_transform(current_point, &trans->dst_matrix[M00]);
                const FIXED point_y = _vertex_transform(current_point, &trans->dst_matrix[M10]);

                current_point += 3;

                const FIXED inv_z = cpu_divu_quotient_get();

                trans_proj->screen.x = fix16_int16_muls(point_x, inv_z);
                trans_proj->screen.y = fix16_int16_muls(point_y, inv_z);
 
                trans_proj++;
        } while (current_point <= last_point);
}

static void
_sort_iterate(sort_single_t *single)
{
        /* No need to clear the end bit, as setting the "source" clobbers the
         * bit */
        _state.current_orderlist->cmdt = single->packet;

        _state.current_orderlist++;
}

static __noinline __used void
_vertex_pool_clipping(transform_t *trans, uint32_t vertex_count)
{
        transform_proj_t *trans_proj;
        trans_proj = &trans->proj_pool[0];

        const int16_t sw_2 = _state.cached_sw_2;
        const int16_t sw_n2 = -_state.cached_sw_2;
        const int16_t sh_2 = _state.cached_sh_2;
        const int16_t sh_n2 = -_state.cached_sh_2;        

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
_polygon_process(const sega3d_object_t *object, transform_t *trans,
    POLYGON const *polygons, uint32_t polygon_count)
{
        for (trans->index = 0; trans->index < polygon_count; trans->index++, polygons++) {
                const uint16_t * vertices = &polygons->Vertices[0];

                trans->polygon[0] = &trans->proj_pool[*(vertices++)];
                trans->polygon[1] = &trans->proj_pool[*(vertices++)];
                trans->polygon[2] = &trans->proj_pool[*(vertices++)];
                trans->polygon[3] = &trans->proj_pool[*vertices];

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

                const FIXED z_avg = trans->polygon[0]->point_z +
                                    trans->polygon[1]->point_z +
                                    trans->polygon[2]->point_z +
                                    trans->polygon[3]->point_z;

                /* Divide by 4 to get the average (bit shift) */
                trans->z_center = z_avg >> 2;

                _cmdt_prepare(object, trans, _state.current_cmdt);

                _internal_sort_add(_state.current_cmdt, fix16_int32_to(trans->z_center) >> 4);

                _state.current_cmdt++;
        }
}

static void
_cmdt_prepare(const sega3d_object_t *object, const transform_t *trans, vdp1_cmdt_t *cmdt)
{
        const PDATA * const pdata = object->pdata;

        const ATTR *attr;
        attr = &pdata->attbl[trans->index];

        cmdt->cmd_ctrl = attr->dir; /* We care about (Dir) and (Comm) bits */
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = attr->atrb;
        cmdt->cmd_colr = attr->colno;

        const sega3d_flags_t debug_flags =
            (SEGA3D_OBJECT_FLAGS_WIREFRAME | SEGA3D_OBJECT_FLAGS_NON_TEXTURED);

        /* For debugging */
        if ((object->flags & debug_flags) != SEGA3D_OBJECT_FLAGS_NONE) {
                cmdt->cmd_ctrl = 0x0004 | ((object->flags & debug_flags) >> 1);
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

        /* Even when there is not texture list, there is the default texture
         * that zeroes out CMDSRCA and CMDSIZE */
        /* if ((attr->sort & UseTexture) == UseTexture) {  */
        /*         const TEXTURE * const texture = sega3d_tlist_tex_get(attr->texno); */

        /*         cmdt->cmd_srca = texture->CGadr; */
        /*         cmdt->cmd_size = texture->HVsize; */
        /* } */

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

        if ((_state.flags & FLAGS_FOG_ENABLED) != FLAGS_NONE) {
                _fog_calculate(trans, cmdt);
        }
}

static __hot void
_fog_calculate(const transform_t *trans, vdp1_cmdt_t *cmdt)
{
        if (trans->z_center < _state.fog_start_z) {
                cmdt->cmd_colr = _state.fog.near_ambient_color.raw;

                return;
        }

        if (trans->z_center >= _state.fog_end_z) {
                cmdt->cmd_colr = _state.fog.far_ambient_color.raw;

                return;
        }

        int32_t int_z_depth;
        int_z_depth = fix16_int16_muls(trans->z_center, _state.fog.step); 

        if (int_z_depth < 0) {
                int_z_depth = 0;
        }
        if (int_z_depth >= _state.fog_len) {
                int_z_depth = _state.fog_len - 1;
        }

        const int32_t depth_index = _depth_fog_z[int_z_depth];

        cmdt->cmd_colr = _depth_fog_colors[depth_index].raw;
}

static __hot bool
_screen_cull_test(const transform_t *trans __unused)
{
        const int16_vec2_t * const p0 = &trans->polygon[0]->screen;
        const int16_vec2_t * const p1 = &trans->polygon[1]->screen;
        const int16_vec2_t * const p2 = &trans->polygon[2]->screen;

        int16_vec2_t u;
        int16_vec2_t v;

        u.x = p1->x - p0->x;
        u.y = p1->y - p0->y;

        v.x = p2->x - p0->x;
        v.y = p2->y - p0->y;

        const int16_t z = (u.x * v.y) - (u.y * v.x);

        return (z >= 0);
}

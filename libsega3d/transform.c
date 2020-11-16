#include <cpu/divu.h>
#include <vdp.h>

#include "sega3d-internal.h"

extern void _internal_sort_clear(void);
extern void _internal_sort_add(void *packet, int32_t pz);
extern void _internal_sort_iterate(iterate_fn fn);

static void _sort_iterate(sort_single_t *single);
static void _vertex_pool_transform(const transform_t * const trans, const POINT * const points);
static void _vertex_pool_clipping(const transform_t * const trans);
static void _polygon_process(transform_t *trans, POLYGON const *polygons);
static void _cmdt_prepare(const transform_t * const trans);
static void _fog_calculate(const transform_t * const trans);
static bool _screen_cull_test(const transform_t * const trans);

static vdp1_cmdt_t _cmdt_end;

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
_internal_transform_init(void)
{
        vdp1_cmdt_end_set(&_cmdt_end);
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

        vdp1_sync_cmdt_orderlist_put(trans->orderlist, NULL, NULL);

        if (results != NULL) {
                results->count = trans->current_orderlist - trans->orderlist;
        }
}

void
sega3d_object_transform(const sega3d_object_t *object)
{
        const PDATA * const pdata = object->pdata;

        const uint16_t polygon_count =
            (pdata->nbPolygon < (PACKET_SIZE - 1)) ? pdata->nbPolygon : (PACKET_SIZE - 1);
        const uint16_t vertex_count =
            (pdata->nbPoint < VERTEX_POOL_SIZE) ? pdata->nbPoint : VERTEX_POOL_SIZE;

        if ((vertex_count == 0) || (polygon_count == 0)) {
                return;
        }

        transform_t * const trans = _internal_state->transform;

        trans->object = object;
        trans->vertex_count = vertex_count;
        trans->polygon_count = polygon_count;
        trans->dst_matrix = (const FIXED *)sega3d_matrix_top();

        _vertex_pool_transform(trans, pdata->pntbl);
        _vertex_pool_clipping(trans);
        _polygon_process(trans, pdata->pltbl);
}

static __noinline __used void
_vertex_pool_transform(const transform_t * const trans, const POINT * const points)
{
        const FIXED *current_point = (const FIXED *)points;
        const FIXED *last_point = (const FIXED *)points[trans->vertex_count];

        transform_proj_t *trans_proj;
        trans_proj = &_internal_state->transform_proj_pool[0];

        const FIXED inv_right = trans->cached_inv_right;
        const FIXED z_near = _internal_state->info->near;

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
_polygon_process(transform_t *trans, POLYGON const *polygons)
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

                const FIXED z_avg = trans->polygon[0]->point_z +
                                    trans->polygon[1]->point_z +
                                    trans->polygon[2]->point_z +
                                    trans->polygon[3]->point_z;

                /* Divide by 4 to get the average (bit shift) */
                trans->z_center = z_avg >> 2;

                _cmdt_prepare(trans);

                _internal_sort_add(trans->current_cmdt, fix16_int32_to(trans->z_center) >> 4);

                trans->current_cmdt++;
        }
}

static void
_cmdt_prepare(const transform_t * const trans)
{
        const sega3d_object_t * const object = trans->object;
        const PDATA * const pdata = object->pdata;

        vdp1_cmdt_t * const cmdt = trans->current_cmdt;

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

        if ((_internal_state->flags & FLAGS_FOG_ENABLED) != FLAGS_NONE) {
                _fog_calculate(trans);
        }
}

static void
_fog_calculate(const transform_t * const trans)
{
        vdp1_cmdt_t * const cmdt = trans->current_cmdt;

        if (trans->z_center < _internal_state->fog->start_z) {
                cmdt->cmd_colr = _internal_state->fog->near_ambient_color.raw;

                return;
        }

        if (trans->z_center >= _internal_state->fog->end_z) {
                cmdt->cmd_colr = _internal_state->fog->far_ambient_color.raw;

                return;
        }

        int32_t int_z_depth;
        int_z_depth = fix16_int16_muls(trans->z_center, _internal_state->fog->step); 

        if (int_z_depth < 0) {
                int_z_depth = 0;
        }
        if (int_z_depth >= _internal_state->fog->depth_count) {
                int_z_depth = _internal_state->fog->depth_count - 1;
        }

        const int32_t depth_index = _internal_state->fog->depth_z[int_z_depth];

        cmdt->cmd_colr = _internal_state->fog->depth_colors[depth_index].raw;
}

static bool
_screen_cull_test(const transform_t * const trans)
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

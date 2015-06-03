/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#define ASSERT                  1
#define FIXMATH_NO_OVERFLOW     1
#define FIXMATH_NO_ROUNDING     1

#include <yaul.h>
#include <fixmath.h>
#include <tga.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/queue.h>

#include "test.h"
#include "common.h"
#include "matrix_stack.h"

typedef struct {
        uint16_t m_index;
#define MATERIAL_FLAG_SINGLE_COLOR      0x01
#define MATERIAL_FLAG_SHADING           0x02
        uint16_t m_flags;
        uint16_t m_single_color;
        int16_t m_shading_index;
} material_t;

#define OT_PRIMITIVE_BUCKETS    1
#define OT_PRIMITIVE_CNT        VDP1_CMDT_COUNT_MAX

#define OT_PRIMITIVE_BUCKET_SORT_INSERTION      0
#define OT_PRIMITIVE_BUCKET_SORT_BUBBLE         1
#define OT_PRIMITIVE_BUCKET_SORT_QUICK          2

struct ot_primitive {
        material_t *otp_material;
        fix16_t otp_avg;
        int16_vector2_t otp_coords[4];

        TAILQ_ENTRY(ot_primitive) otp_entries;
} __aligned(32);

struct ot_primitive_bucket {
        TAILQ_HEAD(, ot_primitive) opb_bucket;
        uint32_t opb_count;
};

/* OT */
static struct ot_primitive_bucket ot_primitive_bucket_pool[OT_PRIMITIVE_BUCKETS] __unused;
static struct ot_primitive ot_primitive_pool[OT_PRIMITIVE_CNT] __unused;

static uint32_t ot_primitive_pool_idx;

static void ot_init(void);
static void ot_primitive_add(const fix16_vector4_t *, const fix16_vector4_t *,
    material_t *);
static void ot_bucket_init(int32_t);
static void ot_bucket_primitive_sort(int32_t, int32_t);


static void model_polygon_project(const fix16_vector4_t *, const uint32_t *,
    const fix16_vector4_t *, const material_t *, const uint32_t);


#define MODEL_TRANSFORMATIONS   1 /* 0: No transforms   1: Apply transforms */
#define MODEL_PROJECT           1 /* 0: Upload          1: Upload and project */
#define POLYGON_SORT            1 /* 0: No sort         1: Sort */
#define RENDER                  1 /* 0: No render       1: Render */
#define CULLING                 0 /* 0: No culling      1: Culling */


static struct vdp1_cmdt_polygon __unused polygon;

static fix16_vector4_t __unused *cube2_vertices;
static fix16_vector4_t __unused *cube2_normals;
static uint32_t __unused *cube2_indices;
static material_t __unused *cube2_materials;
static uint32_t __unused cube2_face_count;

static fix16_vector4_t __unused *teapot2_vertices;
static fix16_vector4_t __unused *teapot2_normals;
static uint32_t __unused *teapot2_indices;
static uint32_t __unused teapot2_face_count;

static fix16_vector4_t __unused *cone2_vertices;
static fix16_vector4_t __unused *cone2_normals;
static uint32_t __unused *cone2_indices;
static material_t __unused *cone2_materials;
static uint32_t __unused cone2_face_count;

static fix16_vector4_t __unused cube_vertices[] = {
        FIX16_VERTEX4(-0.25f,  0.25f,  0.25f, 1.0f),
        FIX16_VERTEX4( 0.25f,  0.25f,  0.25f, 1.0f),
        FIX16_VERTEX4( 0.25f, -0.25f,  0.25f, 1.0f),
        FIX16_VERTEX4(-0.25f, -0.25f,  0.25f, 1.0f),

        FIX16_VERTEX4(-0.25f,  0.25f, -0.25f, 1.0f),
        FIX16_VERTEX4( 0.25f,  0.25f, -0.25f, 1.0f),
        FIX16_VERTEX4( 0.25f, -0.25f, -0.25f, 1.0f),
        FIX16_VERTEX4(-0.25f, -0.25f, -0.25f, 1.0f)
};

static uint32_t
read_model(const char *path, fix16_vector4_t **vertices, fix16_vector4_t __unused **normals,
    uint32_t **indices, material_t **materials)
{
        static uint32_t gouraud_offset = 0;

        void *fh;
        fh = fs_open(path);

        uint8_t *ptr;
        ptr = (uint8_t *)0x00200000;

        fs_read(fh, ptr);
        fs_close(fh);

        typedef struct {
                const char sig[3];
                uint8_t ver;
                uint32_t vertices_cnt;
                uint32_t normals_cnt;
                uint32_t faces_cnt;
                uint32_t materials_cnt;
        } __packed _3do_t;

        _3do_t *_3do_file;
        _3do_file = (_3do_t *)ptr;

        size_t vertices_bytes;
        vertices_bytes = _3do_file->vertices_cnt * sizeof(fix16_vector4_t);
        *vertices = (fix16_vector4_t *)malloc(vertices_bytes);
        assert(*vertices != NULL);

        uint32_t vertex_idx;
        for (vertex_idx = 0; vertex_idx < _3do_file->vertices_cnt; vertex_idx++) {
                fix16_vector3_t *vertex;
                vertex = (fix16_vector3_t *)(ptr + sizeof(_3do_t)) + vertex_idx;

                (*vertices)[vertex_idx].x = vertex->x;
                (*vertices)[vertex_idx].y = vertex->y;
                (*vertices)[vertex_idx].z = vertex->z;
                (*vertices)[vertex_idx].w = F16(1.0f);
        }
        size_t actual_vertices_bytes;
        actual_vertices_bytes = _3do_file->vertices_cnt * sizeof(fix16_vector3_t);

        size_t normals_bytes;
        normals_bytes = _3do_file->normals_cnt * sizeof(fix16_vector4_t);
        *normals = (fix16_vector4_t *)malloc(normals_bytes);
        assert(*normals != NULL);

        uint32_t normal_idx;
        for (normal_idx = 0; normal_idx < _3do_file->normals_cnt; normal_idx++) {
                fix16_vector3_t *normal;
                normal = (fix16_vector3_t *)(ptr + sizeof(_3do_t) + actual_vertices_bytes) + normal_idx;
                (*normals)[normal_idx].x = normal->x;
                (*normals)[normal_idx].y = normal->y;
                (*normals)[normal_idx].z = normal->z;
                (*normals)[normal_idx].w = F16(1.0f);
        }
        size_t actual_normals_bytes;
        actual_normals_bytes = _3do_file->normals_cnt * sizeof(fix16_vector3_t);

        size_t indices_bytes;
        indices_bytes = 5 * _3do_file->faces_cnt * sizeof(uint32_t);
        *indices = (uint32_t *)malloc(indices_bytes);
        assert(*indices != NULL);
        size_t actual_indices_bytes;
        actual_indices_bytes = 5 * _3do_file->faces_cnt * sizeof(uint16_t);

        typedef struct {
                uint16_t material_idx;
                uint16_t vertex_idx[4];
        } __packed _face_t;

        typedef struct {
                uint16_t flags;
                uint16_t next;
                uint16_t single_color;
                uint16_t colors[4];
        } __packed _material_t;

        size_t materials_bytes;
        materials_bytes = _3do_file->materials_cnt * sizeof(material_t);
        *materials = (material_t *)malloc(materials_bytes);
        assert(*materials != NULL);

        _material_t *material;
        material = (_material_t *)(ptr + sizeof(_3do_t) + actual_vertices_bytes + actual_normals_bytes + actual_indices_bytes);
        uint16_t material_idx;
        for (material_idx = 0; ; material_idx++) {
                (*materials)[material_idx].m_index = material_idx;
                (*materials)[material_idx].m_flags = material->flags;
                (*materials)[material_idx].m_single_color = material->single_color;

                (*materials)[material_idx].m_shading_index = -1;
                if ((material->flags & MATERIAL_FLAG_SHADING) == MATERIAL_FLAG_SHADING) {
                        (*materials)[material_idx].m_shading_index = gouraud_offset;

                        /* Copy */
                        memcpy((uint16_t *)GOURAUD(gouraud_offset, 0), material->colors, 4 * sizeof(uint16_t));
                        gouraud_offset++;
                }

                if (material->next == 0xFFFF) {
                        break;
                }
                material = (_material_t *)((uintptr_t)material + (material->next * sizeof(uint16_t)));
        }

        uint32_t face_idx;
        for (face_idx = 0; face_idx < _3do_file->faces_cnt; face_idx++) {
                _face_t *face;
                face = (_face_t *)(ptr + sizeof(_3do_t) + actual_vertices_bytes + actual_normals_bytes) + face_idx;

                (*indices)[5 * face_idx] = face->material_idx;
                (*indices)[(5 * face_idx) + 1] = face->vertex_idx[0];
                (*indices)[(5 * face_idx) + 2] = face->vertex_idx[1];
                (*indices)[(5 * face_idx) + 3] = face->vertex_idx[2];
                (*indices)[(5 * face_idx) + 4] = face->vertex_idx[3];
        }

        return _3do_file->faces_cnt;
}

void
test_07_init(void)
{
        init();

        cone2_face_count = read_model("/TESTS/07/CONE.3DO",
            &cone2_vertices,
            &cone2_normals,
            &cone2_indices,
            &cone2_materials);

        ot_init();
        matrix_stack_init();

        matrix_stack_mode(MATRIX_STACK_MODE_PROJECTION);

        fix16_t ratio;
        ratio = F16((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
        ratio = F16(1.4f);

        matrix_stack_orthographic_project(-fix16_mul(F16(100.0f), ratio),
            fix16_mul(F16(100.0f), ratio),
            F16(100.0f), F16(-100.0f),
            F16(100.0f), F16(100.0f));

        matrix_stack_mode(MATRIX_STACK_MODE_MODEL_VIEW);
        matrix_stack_translate(F16(0.0f), F16(0.0f), F16(-10.0f));

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_local_coord_set(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);

        memset(&polygon, 0x00, sizeof(polygon));
 }

void
test_07_update(void)
{
        static fix16_t angle __unused = F16(0.0f);

        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        return;
                }
        }

        cons_buffer(&cons, "[H[2J");

        uint16_t start_tick;
        start_tick = tick;

        uint16_t start_scanline;
        start_scanline = vdp2_tvmd_vcount_get();

        matrix_stack_mode(MATRIX_STACK_MODE_MODEL_VIEW);
        matrix_stack_push(); {
                matrix_stack_scale(F16(100.0f), F16(100.0f), F16(100.0f));
                matrix_stack_translate(F16(-0.85f), F16(0.0f), F16(0.0f));
                matrix_stack_rotate(angle, 0);
                matrix_stack_rotate(angle, 2);

                /* model_polygon_project(cube2_vertices, cube2_indices, */
                /*     cube2_normals, cube2_face_count); */
                /* model_polygon_project(teapot2_vertices, teapot2_indices, */
                /*     teapot2_normals, teapot2_face_count); */
        } matrix_stack_pop();

        matrix_stack_mode(MATRIX_STACK_MODE_MODEL_VIEW);
        matrix_stack_push(); {
                matrix_stack_scale(F16(50.0f), F16(50.0f), F16(50.0f));
                /* matrix_stack_translate(F16(0.50f), F16(0.0f), F16(0.0f)); */
                matrix_stack_rotate(angle, 0);
                matrix_stack_rotate(angle, 1);
                matrix_stack_rotate(angle, 2);

                model_polygon_project(cone2_vertices, cone2_indices,
                    cone2_normals, cone2_materials, cone2_face_count);
        } matrix_stack_pop();

        angle = fix16_add(angle, F16(-1.0f));

        int32_t buckets;
        buckets = 0;

        int32_t polygons;
        polygons = 0;

        vdp1_cmdt_list_begin(1); {
                int32_t idx;
                for (idx = OT_PRIMITIVE_BUCKETS - 1; idx >= 0; idx--) {
                        /* Skip empty buckets */
                        if (TAILQ_EMPTY(&ot_primitive_bucket_pool[idx].opb_bucket)) {
                                continue;
                        }

                        buckets++;

#if POLYGON_SORT == 1
                        ot_bucket_primitive_sort(idx & (OT_PRIMITIVE_BUCKETS - 1),
                            OT_PRIMITIVE_BUCKET_SORT_INSERTION);
#endif
#if RENDER == 1
                        struct ot_primitive *otp;
                        TAILQ_FOREACH (otp, &ot_primitive_bucket_pool[idx].opb_bucket, otp_entries) {
                                polygons++;

                                polygon.cp_color = otp->otp_material->m_single_color;
                                if ((otp->otp_material->m_flags & MATERIAL_FLAG_SHADING) == MATERIAL_FLAG_SHADING) {
                                        polygon.cp_mode.cc_mode = 4;
                                        polygon.cp_grad = GOURAUD(otp->otp_material->m_shading_index, 0);
                                }
                                polygon.cp_mode.transparent_pixel = true;
                                polygon.cp_mode.end_code = true;

                                polygon.cp_vertex.a.x = otp->otp_coords[0].x;
                                polygon.cp_vertex.a.y = otp->otp_coords[0].y;
                                polygon.cp_vertex.b.x = otp->otp_coords[1].x;
                                polygon.cp_vertex.b.y = otp->otp_coords[1].y;
                                polygon.cp_vertex.c.x = otp->otp_coords[2].x;
                                polygon.cp_vertex.c.y = otp->otp_coords[2].y;
                                polygon.cp_vertex.d.x = otp->otp_coords[3].x;
                                polygon.cp_vertex.d.y = otp->otp_coords[3].y;

                                vdp1_cmdt_polygon_draw(&polygon);
                        }
#endif
                        /* Clear OT bucket */
                        ot_bucket_init(idx);
                }
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(1);

        uint16_t end_tick;
        end_tick = tick;

        uint16_t end_scanline;
        end_scanline = vdp2_tvmd_vcount_get();

        (void)sprintf(text, "Tick diff: %i-%i=%i\n"
            "Scanline diff: %i-%i=%i\n"
            "Buckets: %i\n"
            "Polygons: %i/%i\n",
            (int)end_tick,
            (int)start_tick,
            (int)(end_tick - start_tick),
            (int)end_scanline,
            (int)start_scanline,
            (((end_tick - start_tick) == 0)
                ? (int)(end_scanline - start_scanline)
                : 0),
            (int)buckets,
            (int)polygons,
            (int)teapot2_face_count);
        cons_buffer(&cons, text);
}

void
test_07_draw(void)
{
        cons_flush(&cons);

        vdp1_cmdt_list_commit();
}

void
test_07_exit(void)
{
}

static void __unused
model_polygon_project(const fix16_vector4_t *vb, const uint32_t *ib,
    const fix16_vector4_t *nb __unused,
    const material_t *materials,
    const uint32_t ib_cnt)
{
        fix16_vector4_t vertex_mv[4];
        fix16_vector4_t vertex_projected[4];

        fix16_matrix4_t *matrix_projection;
        matrix_projection = matrix_stack_top(
                MATRIX_STACK_MODE_PROJECTION)->ms_matrix;

        fix16_matrix4_t *matrix_model_view;
        matrix_model_view = matrix_stack_top(
                MATRIX_STACK_MODE_MODEL_VIEW)->ms_matrix;

        /* PVMv = (P(VM))v */

        /* Calculate world to object space matrix (inverse) */
        fix16_matrix4_t matrix_wo;
        fix16_matrix4_inverse(matrix_model_view, &matrix_wo);

        fix16_vector4_t view_forward;
        view_forward.x = F16(0.0f);
        view_forward.y = F16(0.0f);
        view_forward.z = F16(-1.0f);
        view_forward.w = F16(1.0f);

        fix16_vector4_t view_forward_object;
        fix16_vector4_matrix4_multiply(&matrix_wo, &view_forward,
            &view_forward_object);

        uint32_t idx;
        for (idx = 0; idx < (ib_cnt * 5); idx += 5)
        {
                material_t *material;
                material = (material_t *)&materials[ib[idx]];

                const fix16_vector4_t *vtx[4];

                /* Submit each vertex in this order: A, B, C, D */
                /* Vertex A */ vtx[0] = &vb[ib[idx + 1]];
                /* Vertex B */ vtx[1] = &vb[ib[idx + 2]];
                /* Vertex C */ vtx[2] = &vb[ib[idx + 3]];
                /* Vertex D */ vtx[3] = &vb[ib[idx + 4]];

#if CULLING == 1
                /* Face culling */
                fix16_t dot;
                dot = fix16_vector4_dot(&nb[idx >> 2], &view_forward_object);
                if (dot < F16(0.0f)) {
                        continue;
                }
#endif

                fix16_vector4_matrix4_multiply(matrix_model_view, vtx[0],
                    &vertex_mv[0]);
                fix16_vector4_matrix4_multiply(matrix_model_view, vtx[1],
                    &vertex_mv[1]);
                fix16_vector4_matrix4_multiply(matrix_model_view, vtx[2],
                    &vertex_mv[2]);
                fix16_vector4_matrix4_multiply(matrix_model_view, vtx[3],
                    &vertex_mv[3]);

                fix16_vector4_matrix4_multiply(matrix_projection, &vertex_mv[0],
                    &vertex_projected[0]);
                fix16_vector4_matrix4_multiply(matrix_projection, &vertex_mv[1],
                    &vertex_projected[1]);
                fix16_vector4_matrix4_multiply(matrix_projection, &vertex_mv[2],
                    &vertex_projected[2]);
                fix16_vector4_matrix4_multiply(matrix_projection, &vertex_mv[3],
                    &vertex_projected[3]);

                ot_primitive_add(vertex_mv, vertex_projected, material);
        }
}

static void __unused
ot_init(void)
{
        int32_t bucket_idx;
        for (bucket_idx = 0; bucket_idx < OT_PRIMITIVE_BUCKETS; bucket_idx++) {
                ot_bucket_init(bucket_idx);
        }

        ot_primitive_pool_idx = 0;
}

static void __unused
ot_bucket_init(int32_t idx)
{
        struct ot_primitive_bucket *opb;
        opb = &ot_primitive_bucket_pool[idx & (OT_PRIMITIVE_BUCKETS - 1)];

        TAILQ_INIT(&opb->opb_bucket);
        opb->opb_count = 0;
}

static void __unused
ot_primitive_add(const fix16_vector4_t *vertex_mv,
    const fix16_vector4_t *vertex_projected, material_t *material)
{
        struct ot_primitive *otp;
        otp = &ot_primitive_pool[ot_primitive_pool_idx];

        ot_primitive_pool_idx++;
        ot_primitive_pool_idx &= OT_PRIMITIVE_CNT - 1;

        fix16_t avg;
        avg = fix16_mul(fix16_add(
                    fix16_add(vertex_mv[0].z, vertex_mv[1].z),
                    fix16_add(vertex_mv[2].z, vertex_mv[3].z)),
            F16(1.0f / 4.0f));

        otp->otp_material = material;
        otp->otp_avg = avg;

        fix16_t half_width;
        half_width = F16((float)SCREEN_WIDTH / 2.0f);

        fix16_t half_height;
        half_height = F16((float)-SCREEN_HEIGHT / 2.0f);

        otp->otp_coords[0].x = fix16_to_int(
                fix16_mul(vertex_projected[0].x, half_width));
        otp->otp_coords[0].y = fix16_to_int(
                fix16_mul(vertex_projected[0].y, half_height));
        otp->otp_coords[1].x = fix16_to_int(
                fix16_mul(vertex_projected[1].x, half_width));
        otp->otp_coords[1].y = fix16_to_int(
                fix16_mul(vertex_projected[1].y, half_height));
        otp->otp_coords[2].x = fix16_to_int(
                fix16_mul(vertex_projected[2].x, half_width));
        otp->otp_coords[2].y = fix16_to_int(
                fix16_mul(vertex_projected[2].y, half_height));
        otp->otp_coords[3].x = fix16_to_int(
                fix16_mul(vertex_projected[3].x, half_width));
        otp->otp_coords[3].y = fix16_to_int(
                fix16_mul(vertex_projected[3].y, half_height));
        /* Screen coordinates */

        fix16_t abs_avg;
        abs_avg = fix16_abs(avg);

        int32_t idx;
        /* XXX */
        idx = fix16_to_int(abs_avg);

        struct ot_primitive_bucket *opb;
        opb = &ot_primitive_bucket_pool[idx & (OT_PRIMITIVE_BUCKETS - 1)];

        TAILQ_INSERT_HEAD(&opb->opb_bucket, otp, otp_entries);
        opb->opb_count++;
}

static void __unused
ot_bucket_primitive_sort(int32_t idx, int32_t type)
{
        struct ot_primitive *head;
        head = NULL;

        struct ot_primitive *safe;
        struct ot_primitive *otp;

        if (type == OT_PRIMITIVE_BUCKET_SORT_INSERTION) {
                for (otp = TAILQ_FIRST(&ot_primitive_bucket_pool[idx].opb_bucket);
                     (otp != NULL) && (safe = TAILQ_NEXT(otp, otp_entries), 1);
                     otp = safe) {
                        struct ot_primitive *otp_current;
                        otp_current = otp;
                        if ((head == NULL) || (otp_current->otp_avg > head->otp_avg)) {
                                TAILQ_NEXT(otp_current, otp_entries) = head;
                                head = otp_current;
                                TAILQ_FIRST(&ot_primitive_bucket_pool[idx].opb_bucket) = head;
                                continue;
                        }

                        struct ot_primitive *otp_p;
                        for (otp_p = head; otp_p != NULL; ) {
                                struct ot_primitive **otp_p_next;
                                otp_p_next = &TAILQ_NEXT(otp_p, otp_entries);

                                if ((*otp_p_next == NULL) ||
                                    (otp_current->otp_avg > (*otp_p_next)->otp_avg)) {
                                        TAILQ_NEXT(otp_current, otp_entries) =
                                            *otp_p_next;
                                        *otp_p_next = otp_current;
                                        break;
                                }

                                otp_p = *otp_p_next;
                        }
                }
        } else if (type == OT_PRIMITIVE_BUCKET_SORT_BUBBLE) {
                bool swapped;
                do {
                        swapped = false;

                        struct ot_primitive *otp_k __unused;
                        otp_k = NULL;

                        head = TAILQ_FIRST(&ot_primitive_bucket_pool[idx].opb_bucket);
                        for (otp = head;
                             (otp != NULL) && (safe = TAILQ_NEXT(otp, otp_entries), 1);
                             otp = safe) {
                                struct ot_primitive *otp_i;
                                otp_i = otp;

                                struct ot_primitive *otp_j;
                                otp_j = TAILQ_NEXT(otp, otp_entries);

                                if (otp_j == NULL) {
                                        continue;
                                }

                                if (otp_j->otp_avg > otp_i->otp_avg) {
                                        if (otp_k == NULL) {
                                                TAILQ_FIRST(&ot_primitive_bucket_pool[idx].opb_bucket) =
                                                    otp_j;
                                        } else {
                                                TAILQ_NEXT(otp_k, otp_entries) =
                                                    otp_j;
                                        }

                                        TAILQ_NEXT(otp_i, otp_entries) =
                                            TAILQ_NEXT(otp_j, otp_entries);
                                        TAILQ_NEXT(otp_j, otp_entries) = otp_i;

                                        swapped = true;
                                }

                                otp_k = otp_i;
                        }
                } while (swapped);
        } else if (type == OT_PRIMITIVE_BUCKET_SORT_QUICK) {
        }
}

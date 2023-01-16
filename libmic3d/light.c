#include "internal.h"

#include <fix16.h>

static void _polygon_process(void);
static void _polygon_passthrough_process(void);

void
__light_init(void)
{
        light_gst_set(NULL, 0, VDP1_VRAM(0x00000000));

        fix16_mat33_zero(&__state.light->color_matrix);
        fix16_mat33_zero(&__state.light->light_matrix);

        __state.light->light_count = 0;

        // XXX: Testing
        __state.light->color_matrix.frow[0][0] = FIX16(31);
        __state.light->color_matrix.frow[1][0] = FIX16(31);
        __state.light->color_matrix.frow[2][0] = FIX16(31);

        __state.light->color_matrix.frow[0][1] = FIX16(31);
        __state.light->color_matrix.frow[1][1] = FIX16( 0);
        __state.light->color_matrix.frow[2][1] = FIX16( 0);

        __state.light->color_matrix.frow[0][2] = FIX16(31);
        __state.light->color_matrix.frow[1][2] = FIX16( 0);
        __state.light->color_matrix.frow[2][2] = FIX16( 0);

        __state.light->light_matrix.row[0].x = FIX16_ZERO;
        __state.light->light_matrix.row[0].y = FIX16_ZERO;
        __state.light->light_matrix.row[0].z = FIX16_ONE;
        __state.light->light_count = 1;
}

static void
_world_matrix_invert(fix16_mat33_t *inv_matrix)
{
        const fix16_mat43_t * const world_matrix = matrix_top();

        /* Invert here directly to a 3x3 matrix. If we use fix16_mat43_invert,
         * the translation vector is also inverted.
         *
         * The transpose also bakes the negation of the directional light
         * vector: f=dot(vn,-dir) */
        inv_matrix->frow[0][0] = -world_matrix->frow[0][0];
        inv_matrix->frow[0][1] = -world_matrix->frow[1][0];
        inv_matrix->frow[0][2] = -world_matrix->frow[2][0];

        inv_matrix->frow[1][0] = -world_matrix->frow[0][1];
        inv_matrix->frow[1][1] = -world_matrix->frow[1][1];
        inv_matrix->frow[1][2] = -world_matrix->frow[2][1];

        inv_matrix->frow[2][0] = -world_matrix->frow[0][2];
        inv_matrix->frow[2][1] = -world_matrix->frow[1][2];
        inv_matrix->frow[2][2] = -world_matrix->frow[2][2];
}

// void light_color_set(light_id_t id, rgb1555_t color)
// void light_color_comp_set(light_id_t id, uint8_t r, uint8_t g, uint8_t b)
//
// void light_dir_set(light_id_t id, const fix16_vec3_t *dir)
//
// void light_toggle(light_id_t id, bool toggle)

void
__light_transform(void)
{
        if (!RENDER_FLAG_TEST(LIGHTING) || (__state.light->light_count == 0)) {
                __state.light->polygon_process = _polygon_passthrough_process;

                return;
        }

        __state.light->polygon_process = _polygon_process;

        fix16_mat33_t inv_world_matrix __aligned(16);

        _world_matrix_invert(&inv_world_matrix);

        fix16_mat33_t world_light_matrix __aligned(16);

        fix16_mat33_mul(&__state.light->light_matrix, &inv_world_matrix, &world_light_matrix);
        fix16_mat33_mul(&__state.light->color_matrix, &world_light_matrix, &__state.light->intensity_matrix);
}

void
__light_polygon_process(void)
{
        __state.light->polygon_process();
}

static void
_polygon_process(void)
{
        render_transform_t * const render_transform =
            __state.render->render_transform;

        const gst_slot_t gst_slot = __light_gst_alloc();
        vdp1_gouraud_table_t * const gst = __light_gst_get(gst_slot);

        render_transform->rw_attribute.shading_slot = __light_shading_slot_calculate(gst_slot);

        for (uint32_t v = 0; v < 4; v++) {
                const fix16_vec3_t * const vertex_normal =
                    &__state.render->mesh->normals[render_transform->indices.p[v]];

                fix16_vec3_t intensity;
                fix16_mat33_vec3_mul(&__state.light->intensity_matrix, vertex_normal, &intensity);

                /* Avoid shifting to the right by 16, then back up by 5
                 * (green) and 10 (blue) */
                const uint16_t r = (intensity.x >> 16) & 0x001F;
                const uint16_t g = (intensity.y >> 11) & 0x03E0;
                const uint16_t b = (intensity.z >>  6) & 0x7C00;

                gst->colors[v].raw = 0x8000 | b | g | r;
        }
}

static void
_polygon_passthrough_process(void)
{
        render_transform_t * const render_transform =
            __state.render->render_transform;

        render_transform->rw_attribute.shading_slot =
            __gst_slot_calculate(render_transform->ro_attribute->shading_slot);
}

void
__light_gst_put(void)
{
        if (__state.light->gst_count == 0) {
                return;
        }

        scu_dma_transfer(0, (void *)__state.light->vram_base, __state.light->gouraud_tables, __state.light->gst_count * sizeof(vdp1_gouraud_table_t));
        scu_dma_transfer_wait(0);

        __state.light->gst_count = 0;
}

void
light_gst_set(vdp1_gouraud_table_t *gouraud_tables, uint32_t count, vdp1_vram_t vram_base)
{
        __state.light->gouraud_tables = gouraud_tables;
        __state.light->count = count;
        __state.light->vram_base = vram_base;
        __state.light->slot_base = vram_base >> 3;

        __state.light->gst_count = 0;
}

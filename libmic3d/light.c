/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <gamemath/fix16.h>

#include "internal.h"

#define MATRIX_INDEX_INVERSE_WORLD 0
#define MATRIX_INDEX_WORLD_LIGHT   1
#define MATRIX_INDEX_LIGHT         2
#define MATRIX_INDEX_COLOR         3
#define MATRIX_INDEX_INTENSITY     4

static void _polygon_process(void);
static void _polygon_passthrough_process(void);

void
__light_init(void)
{
    workarea_mic3d_t * const workarea = __state.workarea;
    light_t * const light = __state.light;

    fix16_mat33_t * const light_matrices = (void *)workarea->light_matrices;

    light->matrices.inv_world = &light_matrices[MATRIX_INDEX_INVERSE_WORLD];
    light->matrices.world_light = &light_matrices[MATRIX_INDEX_WORLD_LIGHT];
    light->matrices.light = &light_matrices[MATRIX_INDEX_LIGHT];
    light->matrices.color = &light_matrices[MATRIX_INDEX_COLOR];
    light->matrices.intensity = &light_matrices[MATRIX_INDEX_INTENSITY];

    light_gst_set(NULL, 0, VDP1_VRAM(0x00000000));

    fix16_mat33_identity(light->matrices.inv_world);
    fix16_mat33_identity(light->matrices.world_light);
    fix16_mat33_zero(light->matrices.color);
    fix16_mat33_zero(light->matrices.light);
    fix16_mat33_zero(light->matrices.intensity);

    light->light_count = 0;

    // XXX: Testing
    light->matrices.color->frow[0][0] = FIX16(31);
    light->matrices.color->frow[1][0] = FIX16(31);
    light->matrices.color->frow[2][0] = FIX16(31);

    light->matrices.color->frow[0][1] = FIX16(31);
    light->matrices.color->frow[1][1] = FIX16( 0);
    light->matrices.color->frow[2][1] = FIX16( 0);

    light->matrices.color->frow[0][2] = FIX16(31);
    light->matrices.color->frow[1][2] = FIX16( 0);
    light->matrices.color->frow[2][2] = FIX16( 0);

    light->matrices.light->row[0].x = FIX16_ZERO;
    light->matrices.light->row[0].y = FIX16_ZERO;
    light->matrices.light->row[0].z = FIX16_ONE;

    light->light_count = 1;

}

static void
_world_matrix_invert(void)
{
    render_t * const render = __state.render;
    light_t * const light = __state.light;

    const fix16_mat43_t * const world_matrix = render->mesh_world_matrix;
    fix16_mat33_t * const inv_world_matrix = light->matrices.inv_world;

    /* Invert here directly to a 3x3 matrix. If we use fix16_mat43_invert,
     * the translation vector is also inverted.
     *
     * The transpose also bakes the negation of the directional light
     * vector: f=dot(vn,-dir) */
    inv_world_matrix->frow[0][0] = -world_matrix->frow[0][0];
    inv_world_matrix->frow[0][1] = -world_matrix->frow[1][0];
    inv_world_matrix->frow[0][2] = -world_matrix->frow[2][0];

    inv_world_matrix->frow[1][0] = -world_matrix->frow[0][1];
    inv_world_matrix->frow[1][1] = -world_matrix->frow[1][1];
    inv_world_matrix->frow[1][2] = -world_matrix->frow[2][1];

    inv_world_matrix->frow[2][0] = -world_matrix->frow[0][2];
    inv_world_matrix->frow[2][1] = -world_matrix->frow[1][2];
    inv_world_matrix->frow[2][2] = -world_matrix->frow[2][2];
}

// void light_color_set(light_id_t id, rgb1555_t color)
// void light_color_comp_set(light_id_t id, uint8_t r, uint8_t g, uint8_t b)
//
// void light_dir_set(light_id_t id, const fix16_vec3_t *dir)
//
// void light_toggle(light_id_t id, bool toggle)

void
__light_transform(light_polygon_processor_t *processor)
{
    light_t * const light = __state.light;

    if (!RENDER_FLAG_TEST(LIGHTING) || (light->light_count == 0)) {
        *processor = _polygon_passthrough_process;
        return;
    }

    *processor = _polygon_process;

    render_t * const render = __state.render;

    _world_matrix_invert();

    fix16_mat33_t * const inv_world_matrix = light->matrices.inv_world;
    fix16_mat33_t * const world_light_matrix = light->matrices.world_light;

    fix16_mat33_mul(light->matrices.light, inv_world_matrix, world_light_matrix);
    fix16_mat33_mul(light->matrices.color, world_light_matrix, light->matrices.intensity);

    for (uint32_t i = 0; i < render->mesh->points_count; i++) {
        const fix16_vec3_t * const vertex_normal = &render->mesh->normals[i];

        /* Avoid shifting to the right by 16, then back up by 5
         * (green) and 10 (blue) */

        uint16_t color;
        color = 0x8000;

        fix16_t intensity;

        intensity = fix16_vec3_dot(&light->matrices.intensity->row[0], vertex_normal);
        color |= ((uint32_t)intensity >> 16) & 0x001F;

        intensity = fix16_vec3_dot(&light->matrices.intensity->row[1], vertex_normal);
        color |= ((uint32_t)intensity >> 11) & 0x03E0;

        intensity = fix16_vec3_dot(&light->matrices.intensity->row[2], vertex_normal);
        color |= ((uint32_t)intensity >>  6) & 0x7C00;

        render->colors_pool[i].raw = color;
    }
}

static void
_polygon_process(void)
{
    render_t * const render = __state.render;
    render_transform_t * const render_transform = render->render_transform;

    const gst_slot_t gst_slot = __light_gst_alloc();
    vdp1_gouraud_table_t * const gst = __light_gst_get(gst_slot);

    render_transform->attribute.shading_slot =
      __light_shading_slot_calculate(gst_slot);

    gst->colors[0] = render->colors_pool[render_transform->polygon.indices.p[0]];
    gst->colors[1] = render->colors_pool[render_transform->polygon.indices.p[1]];
    gst->colors[2] = render->colors_pool[render_transform->polygon.indices.p[2]];
    gst->colors[3] = render->colors_pool[render_transform->polygon.indices.p[3]];
}

static void
_polygon_passthrough_process(void)
{
    render_t * const render = __state.render;
    render_transform_t * const render_transform = render->render_transform;

    render_transform->attribute.shading_slot =
      __gst_slot_calculate(render_transform->attribute.shading_slot);
}

void
__light_gst_put(void)
{
    light_t * const light = __state.light;

    if (light->gst_count == 0) {
        return;
    }

    scu_dma_transfer(0, (void *)light->vram_base, light->gouraud_tables, light->gst_count * sizeof(vdp1_gouraud_table_t));
    scu_dma_transfer_wait(0);

    light->gst_count = 0;
}

void
light_gst_set(vdp1_gouraud_table_t *gouraud_tables, uint32_t count, vdp1_vram_t vram_base)
{
    light_t * const light = __state.light;

    light->gouraud_tables = gouraud_tables;
    light->count = count;
    light->vram_base = vram_base;
    light->slot_base = vram_base >> 3;

    light->gst_count = 0;
}

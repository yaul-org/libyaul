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

    light->colors_pool = (void *)workarea->colors;

    fix16_mat33_t * const light_matrices = (void *)workarea->light_matrices;

    light->inv_world_matrix = &light_matrices[MATRIX_INDEX_INVERSE_WORLD];
    light->world_light_matrix = &light_matrices[MATRIX_INDEX_WORLD_LIGHT];
    light->light_matrix = &light_matrices[MATRIX_INDEX_LIGHT];
    light->color_matrix = &light_matrices[MATRIX_INDEX_COLOR];
    light->intensity_matrix = &light_matrices[MATRIX_INDEX_INTENSITY];

    light_gst_set(NULL, 0, VDP1_VRAM(0x00000000));

    fix16_mat33_identity(light->inv_world_matrix);
    fix16_mat33_identity(light->world_light_matrix);
    fix16_mat33_zero(light->color_matrix);
    fix16_mat33_zero(light->light_matrix);
    fix16_mat33_zero(light->intensity_matrix);

    light->light_count = 0;

    /* XXX: Testing */
    light->color_matrix->frow[0][0] = FIX16(31);
    light->color_matrix->frow[1][0] = FIX16(31);
    light->color_matrix->frow[2][0] = FIX16(31);

    light->color_matrix->frow[0][1] = FIX16(31);
    light->color_matrix->frow[1][1] = FIX16( 0);
    light->color_matrix->frow[2][1] = FIX16( 0);

    light->color_matrix->frow[0][2] = FIX16(31);
    light->color_matrix->frow[1][2] = FIX16( 0);
    light->color_matrix->frow[2][2] = FIX16( 0);

    light->light_matrix->row[0].x = FIX16_ZERO;
    light->light_matrix->row[0].y = FIX16_ZERO;
    light->light_matrix->row[0].z = FIX16_ONE;

    light->light_count = 1;

}

static void
_world_matrix_invert(void)
{
    render_t * const render = __state.render;
    light_t * const light = __state.light;

    const fix16_mat43_t * const world_matrix = render->world_matrix;
    fix16_mat33_t * const inv_world_matrix = light->inv_world_matrix;

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

/* void light_color_set(light_id_t id, rgb1555_t color) */
/* void light_color_comp_set(light_id_t id, uint8_t r, uint8_t g, uint8_t b) */

/* void light_dir_set(light_id_t id, const fix16_vec3_t *dir) */

/* void light_toggle(light_id_t id, bool toggle) */

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

    fix16_mat33_t * const inv_world_matrix = light->inv_world_matrix;
    fix16_mat33_t * const world_light_matrix = light->world_light_matrix;
    fix16_mat33_t * const intensity_matrix = light->intensity_matrix;
    fix16_mat33_t * const light_matrix = light->light_matrix;
    fix16_mat33_t * const color_matrix = light->color_matrix;

    fix16_mat33_mul(light_matrix, inv_world_matrix, world_light_matrix);
    fix16_mat33_mul(color_matrix, world_light_matrix, intensity_matrix);

    const mesh_t * const mesh = render->mesh;

    rgb1555_t *colors_ptr;
    colors_ptr = light->colors_pool;

    for (uint32_t i = 0; i < mesh->points_count; i++) {
        const fix16_vec3_t * const vertex_normal = &mesh->normals[i];

        /* Avoid shifting to the right by 16, then back up by 5
         * (green) and 10 (blue) */

        uint16_t color;
        color = 0x8000;

        fix16_t intensity;

        intensity = fix16_vec3_dot(&intensity_matrix->row[0], vertex_normal);
        color |= ((uint32_t)intensity >> 16) & 0x001F;

        intensity = fix16_vec3_dot(&intensity_matrix->row[1], vertex_normal);
        color |= ((uint32_t)intensity >> 11) & 0x03E0;

        intensity = fix16_vec3_dot(&intensity_matrix->row[2], vertex_normal);
        color |= ((uint32_t)intensity >>  6) & 0x7C00;

        colors_ptr->raw = color;
        colors_ptr++;
    }
}

static void
_polygon_process(void)
{
    light_t * const light = __state.light;
    render_t * const render = __state.render;
    pipeline_t * const pipeline = render->pipeline;

    const gst_slot_t gst_slot = __light_gst_alloc();
    vdp1_gouraud_table_t * const gst = __light_gst_get(gst_slot);

    pipeline->attribute.shading_slot =
      __light_shading_slot_calculate(gst_slot);

    const rgb1555_t * const colors = light->colors_pool;

    gst->colors[0] = colors[pipeline->polygon.indices.p[0]];
    gst->colors[1] = colors[pipeline->polygon.indices.p[1]];
    gst->colors[2] = colors[pipeline->polygon.indices.p[2]];
    gst->colors[3] = colors[pipeline->polygon.indices.p[3]];
}

static void
_polygon_passthrough_process(void)
{
    render_t * const render = __state.render;
    pipeline_t * const pipeline = render->pipeline;

    pipeline->attribute.shading_slot =
      __gst_slot_calculate(pipeline->attribute.shading_slot);
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

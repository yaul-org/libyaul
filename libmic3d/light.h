/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_LIGHT_H_
#define _MIC3D_LIGHT_H_

#include <gamemath/fix16.h>

#include "state.h"

#define LIGHT_COUNT (3)

typedef void (*light_polygon_processor_t)(void);

typedef struct light {
    /* Pools */
    struct {
        rgb1555_t *colors_pool;
    };

    vdp1_gouraud_table_t *gouraud_tables;
    uint32_t count;
    vdp1_vram_t vram_base;
    uint16_t slot_base;

    /* Matrices */
    struct {
        fix16_mat33_t *inv_world_matrix;
        fix16_mat33_t *world_light_matrix;
        /* Light matrix represents the direction of each light */
        fix16_mat33_t *light_matrix;
        /* Color matrix represents each light color (column vectors) */
        fix16_mat33_t *color_matrix;
        fix16_mat33_t *intensity_matrix;
    };

    /* Count of enabled lights */
    uint32_t light_count;
    /* Count of used GSTs */
    uint32_t gst_count;
} __aligned(4) light_t;

static inline uint16_t __always_inline
__light_shading_slot_calculate(gst_slot_t gst_slot)
{
    light_t * const light = __state.light;

    return (light->slot_base + gst_slot);
}

static inline gst_slot_t __always_inline
__light_gst_alloc(void)
{
    light_t * const light = __state.light;

    const gst_slot_t gst_slot = light->gst_count++;

    return gst_slot;
}

static inline vdp1_gouraud_table_t * __always_inline
__light_gst_get(gst_slot_t gst_slot)
{
    light_t * const light = __state.light;

    return &light->gouraud_tables[gst_slot];
}

void __light_init(void);
void __light_transform(light_polygon_processor_t *processor);
void __light_gst_put(void);

#endif /* _MIC3D_LIGHT_H_ */

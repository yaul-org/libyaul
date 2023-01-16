#ifndef MIC3D_TYPES_H
#define MIC3D_TYPES_H

#include <assert.h>

#include <fix16.h>
#include <vdp1.h>

typedef enum render_flags {
        RENDER_FLAGS_NONE     = 0,
        RENDER_FLAGS_LIGHTING = 1 << 0,
        RENDER_FLAGS_NO_CLEAR = 1 << 1,
        RENDER_FLAGS_ALL      = RENDER_FLAGS_LIGHTING |
                                RENDER_FLAGS_NO_CLEAR
} render_flags_t;

typedef uint16_t gst_slot_t;

typedef union indices {
        struct {
                uint16_t p0;
                uint16_t p1;
                uint16_t p2;
                uint16_t p3;
        };

        uint16_t p[4];
} __aligned(4) indices_t;

typedef struct {
        fix16_vec3_t normal;
        indices_t indices;
} __aligned(4) polygon_t;

typedef enum {
        SORT_TYPE_BFR,
        SORT_TYPE_MIN,
        SORT_TYPE_MAX,
        SORT_TYPE_CENTER
} sort_type_t;

typedef enum {
        READ_DIR_NORMAL,
        READ_DIR_H,
        READ_DIR_V,
        READ_DIR_HV
} read_dir_t;

typedef enum {
        PLANE_TYPE_SINGLE,
        PLANE_TYPE_DOUBLE
} plane_type_t;

typedef enum {
        COMMAND_TYPE_SPRITE           = 0,
        COMMAND_TYPE_SCALED_SPRITE    = 1,
        COMMAND_TYPE_DISTORTED_SPRITE = 2,
        COMMAND_TYPE_POLYGON          = 4,
        COMMAND_TYPE_POLYLINE         = 5,
        COMMAND_TYPE_LINE             = 6
} command_type_t;

typedef union {
        struct {
                unsigned int :3;
                sort_type_t sort_type:3;
                plane_type_t plane_type:1;
                unsigned int use_shading:1;
                unsigned int use_lighting:1;
                unsigned int use_texture:1;
                read_dir_t read_dir:2;
                command_type_t command:4;
        } __packed;

        uint16_t raw;
} __packed attribute_control_t;

static_assert(sizeof(attribute_control_t) == 2);

typedef struct {
        attribute_control_t control;

        vdp1_cmdt_draw_mode_t draw_mode;

        union {
                uint16_t vram_index;
                vdp1_color_bank_t color_bank;
                uint16_t raw;
                rgb1555_t base_color;
        } palette;

        uint16_t texture_slot;
        gst_slot_t shading_slot;
} __aligned(4) attribute_t;

static_assert(sizeof(attribute_t) == 12);

typedef struct {
        fix16_vec3_t target;
        fix16_vec3_t up;
        fix16_vec3_t position;
} camera_t;

typedef struct {
        const fix16_vec3_t *points;
        uint32_t points_count;
        const fix16_vec3_t *normals;
        const polygon_t *polygons;
        const attribute_t *attributes;
        uint32_t polygons_count;
} mesh_t;

typedef struct {
        const void *data;
        uint16_t data_size;
        int16_vec2_t dim;
        uint16_t palette_index;
} picture_t;

static_assert(sizeof(picture_t) == 16);

typedef struct {
        uint16_t vram_index;
        uint16_t size;
} texture_t;

static_assert(sizeof(texture_t) == 4);

typedef struct {
        const void *data;
        uint16_t data_size;
} palette_t;

static_assert(sizeof(palette_t) == 8);

#endif /* MIC3D_TYPES_H */

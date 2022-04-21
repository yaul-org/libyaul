/*
 * No copyright.
 */

#ifndef _G3D_INTERNAL_H_
#define _G3D_INTERNAL_H_

#include <stdint.h>

#include "g3d.h"

#include <math.h>
#include <vdp1/cmdt.h>

#define MATRIX_STACK_MAX        (32)
#define SORT_Z_RANGE            (256)
#define PACKET_SIZE             (4096)
#define VERTEX_POOL_SIZE        (1024)
#define DISPLAY_LEVEL_COUNT     (8)
#define FAR_Z                   toFIXED(1024.0f)
#define MIN_FOV_ANGLE           (DEGtoANG(60.0f))
#define MAX_FOV_ANGLE           (DEGtoANG(120.0f))

#define CLIP_PLANE_COUNT        (6)

typedef enum {
        FLAGS_NONE        = 0,
        FLAGS_INITIALIZED = 1 << 0,
        FLAGS_FOG_ENABLED = 1 << 1,
} flags_t;

typedef enum {
        LIST_FLAGS_NONE,
        LIST_FLAGS_USER_ALLOCATED = 1 << 0,
        LIST_FLAGS_ALLOCATED      = 1 << 1
} list_flags_t;

typedef enum {
        CLIP_FLAGS_NONE   = 0,
        CLIP_FLAGS_NEAR   = 1 << 0,
        CLIP_FLAGS_FAR    = 1 << 1,
        CLIP_FLAGS_LEFT   = 1 << 2,
        CLIP_FLAGS_RIGHT  = 1 << 3,
        CLIP_FLAGS_TOP    = 1 << 4,
        CLIP_FLAGS_BOTTOM = 1 << 5,
        CLIP_FLAGS_SIDE   = 1 << 6
} clip_flags_t;

typedef struct {
        FIXED point_z;
        int16_vec2_t screen;
        clip_flags_t clip_flags;
} __aligned(16) transform_proj_t;

static_assert(sizeof(transform_proj_t) == 16);

typedef struct {
        int16_t cached_sw_2;           /* Cached half of screen width */
        int16_t cached_sh_2;           /* Cached half of screen height */
        uint16_t vertex_count;         /* Current vertex count */
        uint16_t polygon_count;        /* Current polygon count */
        uint16_t index;                /* Current polygon index */
        FIXED z_value;                 /* Z value of the current polygon */
        const transform_proj_t *polygon[4]; /* Pointers to the pool that make up the current polygon */

        const g3d_object_t *object;    /* Current object */
        const void *xpdata;            /* Current XPDATA */

        vdp1_cmdt_orderlist_t *current_orderlist;
        vdp1_cmdt_orderlist_t *orderlist;
        vdp1_cmdt_t *current_cmdt;
} __aligned(16) transform_t;

static_assert(sizeof(transform_t) == 64);

typedef struct {
        /* XXX: This group of planes are output */
        fix16_plane_t near_plane;
        fix16_plane_t far_plane;
        fix16_plane_t left_plane;
        fix16_plane_t right_plane;
        fix16_plane_t top_plane;
        fix16_plane_t bottom_plane;

        /* XXX: This group of normals & 2 vectors are input to the output planes
         *      above */
        fix16_vec3_t near_normal;
        fix16_vec3_t far_normal;
        fix16_vec3_t left_normal;
        fix16_vec3_t right_normal;
        fix16_vec3_t top_normal;
        fix16_vec3_t bottom_normal;
        fix16_vec3_t near_d;
        fix16_vec3_t far_d;
} __packed __aligned(4) clip_planes_t;

typedef struct sort_single {
        void *packet;
        struct sort_single *next_single;
} __aligned(8) sort_single_t;

typedef struct {
        sort_single_t *head;
} __aligned(4) sort_list_t;

typedef void (*sort_iterate_fn_t)(const sort_single_t *);

typedef struct {
        list_flags_t flags;
        void *list;
        uint16_t count;
        uint16_t size;
        void *default_element;
} __aligned(4) list_t;

typedef struct {
        flags_t flags;
        g3d_results_t * const results;

        g3d_fog_t * const fog;
        g3d_info_t * const info;
        transform_t * const transform;
        transform_proj_t * const transform_proj_pool;
        MATRIX * const clip_camera;
        clip_planes_t * const clip_planes;
        MATRIX * const matrices;
        sort_list_t * const sort_list;
        sort_single_t * const sort_single_pool;
        list_t * const tlist;
        list_t * const plist;
} __aligned(16) state_t;

extern state_t * const __state;

extern void __list_alloc(list_t *list, uint16_t count);
extern void __list_free(list_t *list);
extern void __list_set(list_t *list, void *list_p, uint16_t count);

#endif /* !_G3D_INTERNAL_H_ */

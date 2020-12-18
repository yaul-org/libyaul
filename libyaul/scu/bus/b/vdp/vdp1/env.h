/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_ENV_H_
#define _VDP1_ENV_H_

#include <stdint.h>
#include <stdbool.h>

#include <vdp1/cmdt.h>

#include <color.h>
#include <int16.h>

__BEGIN_DECLS

/// @defgroup VDP1_ENV_DEFINES
/// @defgroup VDP1_ENV_STRUCTURES
/// @defgroup VDP1_ENV_FUNCTIONS

/// @addtogroup VDP1_ENV_DEFINES
/// @{

/// Not yet documented.
#define VDP1_ENV_ROTATION_0  0
/// Not yet documented.
#define VDP1_ENV_ROTATION_90 1

/// Not yet documented.
#define VDP1_ENV_BPP_16      0
/// Not yet documented.
#define VDP1_ENV_BPP_8       1

/// Not yet documented.
#define VDP1_ENV_COLOR_MODE_PALETTE     0
/// Not yet documented.
#define VDP1_ENV_COLOR_MODE_RGB_PALETTE 1
/// Not yet documented.
#define VDP1_ENV_CMDT_PREAMBLE_COUNT (4)

/// @}

/// @addtogroup VDP1_ENV_STRUCTURES
/// @{

/// @compound
/// @brief Not yet documented.
typedef struct vdp1_env {
        unsigned int :8;

        struct {
                unsigned int :1;
                unsigned int bpp:1;
                unsigned int rotation:1;
                unsigned int color_mode:1;
                unsigned int sprite_type:4;
        } __packed;

        color_rgb1555_t erase_color;

        int16_vec2_t erase_points[2];
} vdp1_env_t;

/// @}

/// @addtogroup VDP1_ENV_FUNCTIONS
/// @{
/// @brief Not yet documented.
extern void vdp1_env_default_init(vdp1_env_t *);

/// @brief Not yet documented.
extern void vdp1_env_default_set(void);
/// @brief Not yet documented.
extern void vdp1_env_set(const vdp1_env_t *);

/// @brief Not yet documented.
extern void vdp1_env_stop(void);

/// @brief Not yet documented.
extern void vdp1_env_preamble_populate(vdp1_cmdt_t *,
    const int16_vec2_t *);

/// @}

__END_DECLS

#endif /* !_VDP1_ENV_H_ */

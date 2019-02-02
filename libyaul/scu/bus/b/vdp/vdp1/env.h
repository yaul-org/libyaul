/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_ENV_H_
#define _VDP1_ENV_H_

#include <stdint.h>
#include <stdbool.h>

#include <color.h>
#include <int16.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ENV_ROTATION_0  0
#define ENV_ROTATION_90 1

#define ENV_BPP_16      0
#define ENV_BPP_8       1

#define ENV_COLOR_MODE_PALETTE          0
#define ENV_COLOR_MODE_RGB_PALETTE      1

struct vdp1_env {
        color_rgb555_t env_erase_color;
        int16_vector2_t env_erase_points[2];
        uint8_t env_bpp;
        uint8_t env_rotation;
        uint8_t env_color_mode;
};

extern void vdp1_env_default_set(void);
extern void vdp1_env_set(const struct vdp1_env *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_VDP1_ENV_H_ */

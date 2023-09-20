/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_MATH3D_H_
#define _YAUL_GAMEMATH_MATH3D_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <gamemath/fix16/fix16_mat43.h>
#include <gamemath/fix16/fix16_vec3.h>
#include <gamemath/int16.h>

__BEGIN_DECLS

/// @addtogroup MATH
/// @defgroup MATH_3D 3D Math
/// @ingroup MATH
/// @{

/// @brief Not yet documented.
typedef struct xform_config {
    /// @brief Not yet documented.
    fix16_t near;
    /// @brief Not yet documented.
    fix16_t far;
    /// @brief Not yet documented.
    fix16_t view_distance;
    /// @brief Not yet documented.
    const fix16_mat43_t *view_matrix;
} xform_config_t;

/// @brief Not yet documented.
typedef struct xform {
    /// @brief Not yet documented.
    int16_vec2_t screen_point;
    /// @brief Not yet documented.
    fix16_t depth_value;
} xform_t;

/// @brief Not yet documented.
///
/// @param screen_width  Not yet documented.
/// @param fov_angle     Not yet documented.
///
/// @returns Not yet documented.
extern fix16_t math3d_view_distance_calc(int16_t screen_width,
    angle_t fov_angle);

/// @brief Not yet documented.
///
/// @param      xform_config Not yet documented.
/// @param      point            Not yet documented.
/// @param[out] result           Not yet documented.
extern void math3d_point_xform(const xform_config_t *xform_config,
    const fix16_vec3_t *point, xform_t *result);

/// @}

__END_DECLS

#endif /* !_YAUL_GAMEMATH_MATH3D_H_ */

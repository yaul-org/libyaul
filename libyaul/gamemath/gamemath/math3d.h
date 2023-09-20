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
/// @defgroup MATH_3D 3D
/// @ingroup MATH
/// @{

/// @brief Not yet documented.
typedef struct transform_config {
    /// @brief Not yet documented.
    fix16_t near;
    /// @brief Not yet documented.
    fix16_t far;
    /// @brief Not yet documented.
    fix16_t view_distance;
    /// @brief Not yet documented.
    const fix16_mat43_t *view_matrix;
} transform_config_t;

/// @brief Not yet documented.
typedef struct transform_result {
    /// @brief Not yet documented.
    int16_vec2_t screen_point;
    /// @brief Not yet documented.
    fix16_t depth_value;
} transform_result_t;

/// @brief Not yet documented.
///
/// @param      transform_config Not yet documented.
/// @param      point            Not yet documented.
/// @param[out] result           Not yet documented.
extern void math3d_point_transform(const transform_config_t *transform_config,
    const fix16_vec3_t *point, transform_result_t *result);

/// @}

__END_DECLS

#endif /* !_YAUL_GAMEMATH_MATH3D_H_ */

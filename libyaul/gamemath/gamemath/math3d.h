/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_MATH3D_H_
#define _YAUL_GAMEMATH_MATH3D_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <gamemath/angle.h>
#include <gamemath/fix16/fix16_mat43.h>
#include <gamemath/fix16/fix16_vec3.h>
#include <gamemath/int16.h>

__BEGIN_DECLS

/// @addtogroup MATH
/// @defgroup MATH_3D 3D Math
/// @ingroup MATH
/// @{

/// @brief Not yet documented.
typedef struct {
    /// @brief Not yet documented.
    const fix16_vec3_t *from;
    /// @brief Not yet documented.
    const fix16_vec3_t *to;
    /// @brief Not yet documented.
    const fix16_vec3_t *up;

    /// @brief Not yet documented.
    fix16_vec3_t *basis_right;
    /// @brief Not yet documented.
    fix16_vec3_t *basis_up;
    /// @brief Not yet documented.
    fix16_vec3_t *basis_forward;
} lookat_t;

/// @brief Not yet documented.
typedef struct xform {
    /// @brief Not yet documented.
    int16_vec2_t screen_point;
    /// @brief Not yet documented.
    fix16_t depth_value;
} xform_t;

// TODO: Not yet used
/// @brief Not yet documented.
typedef struct xforms {
    /// @brief Not yet documented.
    int16_vec2_t* screen_points;
    /// @brief Not yet documented.
    fix16_t* depth_values;
    /// @brief Not yet documented.
    uint32_t count;
} xforms_t;

extern void math3d_lookat(const lookat_t *lookat);

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
/// @param      view_matrix   Not yet documented.
/// @param      view_distance Not yet documented.
/// @param      point         Not yet documented.
/// @param[out] result        Not yet documented.
extern void math3d_point_perspective_xform(const fix16_mat43_t *view_matrix,
    fix16_t view_distance, const fix16_vec3_t *point, xform_t *result);

/// @brief Not yet documented.
///
/// @param      view_matrix Not yet documented.
/// @param      ortho_size  Not yet documented.
/// @param      point       Not yet documented.
/// @param[out] result      Not yet documented.
extern void math3d_point_orthographic_xform(const fix16_mat43_t *view_matrix,
    fix16_t ortho_size, const fix16_vec3_t *point, xform_t *result);

/// @}

__END_DECLS

#endif /* !_YAUL_GAMEMATH_MATH3D_H_ */

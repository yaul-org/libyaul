/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_MAT43_H_
#define _YAUL_GAMEMATH_FIX16_MAT43_H_

#include <gamemath/angle.h>
#include <gamemath/fix16/fix16_vec3.h>
#include <gamemath/fix16/fix16_vec4.h>

__BEGIN_DECLS

/// @addtogroup MATH_FIX16_MATRIX
/// @defgroup MATH_FIX16_MATRIX4X3 4x3
/// @ingroup MATH_FIX16_MATRIX
/// @{

/// @brief Not yet documented.
#define FIX16_MAT43_COLUMNS   (4)

/// @brief Not yet documented.
#define FIX16_MAT43_ROWS      (3)

/// @brief Not yet documented.
#define FIX16_MAT43_ARR_COUNT (FIX16_MAT43_COLUMNS * FIX16_MAT43_ROWS)

/// @cond
union fix16_vec3;
union fix16_mat33;

typedef union fix16_mat33 fix16_mat33_t;
/// @endcond

/// @brief Not yet documented.
///
/// @note Row-major matrix.
typedef union fix16_mat43 {
    /// @brief Not yet documented.
    struct {
        /// @brief Not yet documented. Row 0.
        fix16_t m00, m01, m02, m03;
        /// @brief Not yet documented. Row 1.
        fix16_t m10, m11, m12, m13;
        /// @brief Not yet documented. Row 2.
        fix16_t m20, m21, m22, m33;
    } comp;

    /// @brief Not yet documented.
    fix16_t arr[FIX16_MAT43_ARR_COUNT];
    /// @brief Not yet documented.
    fix16_t frow[FIX16_MAT43_ROWS][FIX16_MAT43_COLUMNS];
    /// @brief Not yet documented.
    fix16_vec4_t row[FIX16_MAT43_ROWS];
} __aligned(4) fix16_mat43_t;

/// @brief Not yet documented.
///
/// @param      m0      Not yet documented.
/// @param[out] result  Not yet documented.
static inline void __always_inline
fix16_mat43_translation_get(const fix16_mat43_t *m0, fix16_vec3_t *result)
{
    result->x = m0->frow[0][3];
    result->y = m0->frow[1][3];
    result->z = m0->frow[2][3];
}

/// @brief Not yet documented.
///
/// @param      t      Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_mat43_translation_set(const fix16_vec3_t *t, fix16_mat43_t *result)
{
    result->frow[0][3] = t->x;
    result->frow[1][3] = t->y;
    result->frow[2][3] = t->z;
}

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      x      Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_mat43_x_translate(const fix16_mat43_t *m0, fix16_t x, fix16_mat43_t *result)
{
    result->frow[0][3] = m0->frow[0][3] + x;
}

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      y      Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_mat43_y_translate(const fix16_mat43_t *m0, fix16_t y, fix16_mat43_t *result)
{
    result->frow[1][3] = m0->frow[1][3] + y;
}

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      z      Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_mat43_z_translate(const fix16_mat43_t *m0, fix16_t z, fix16_mat43_t *result)
{
    result->frow[2][3] = m0->frow[2][3] + z;
}

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param[out] result Not yet documented.
/// @param      t      Not yet documented.
static inline void __always_inline
fix16_mat43_translate(const fix16_mat43_t *m0, const fix16_vec3_t *t, fix16_mat43_t *result)
{
    fix16_mat43_x_translate(m0, t->x, result);
    fix16_mat43_y_translate(m0, t->y, result);
    fix16_mat43_z_translate(m0, t->z, result);
}

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_dup(const fix16_mat43_t *m0, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_invert(const fix16_mat43_t *m0, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      from   Not yet documented.
/// @param      to     Not yet documented.
/// @param      up     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_lookat(const fix16_vec3_t *from, const fix16_vec3_t *to,
    const fix16_vec3_t *up, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      m1     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_mul(const fix16_mat43_t *m0, const fix16_mat43_t *m1, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      angle  Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_x_rotate(const fix16_mat43_t *m0, angle_t angle, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      angle  Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_y_rotate(const fix16_mat43_t *m0, angle_t angle, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      angle  Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_z_rotate(const fix16_mat43_t *m0, angle_t angle, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      rx     Not yet documented.
/// @param      ry     Not yet documented.
/// @param      rz     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_rotation_create(angle_t rx, angle_t ry, angle_t rz, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      r      Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_rotation_set(const fix16_mat33_t *r, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param m0 Not yet documented.
extern void fix16_mat43_identity(fix16_mat43_t *m0);

/// @brief Not yet documented.
///
/// @param      m0       Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documunted.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_mat43_str(const fix16_mat43_t *m0, char *buffer, int32_t decimals);

/// @brief Not yet documented.
///
/// @param m0 Not yet documented.
extern void fix16_mat43_zero(fix16_mat43_t *m0);

/// @}

__END_DECLS

#endif /* !_YAUL_GAMEMATH_FIX16_MAT43_H_ */

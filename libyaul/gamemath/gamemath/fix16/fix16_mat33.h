/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_MAT33_H_
#define _YAUL_GAMEMATH_FIX16_MAT33_H_

#include <gamemath/fix16/fix16_vec3.h>

/// @addtogroup MATH_FIX16_MATRIX
/// @defgroup MATH_FIX16_MATRIX3X3 3x3
/// @ingroup MATH_FIX16_MATRIX
/// @{

/// @brief Not yet documented.
#define FIX16_MAT33_COLUMNS   (3)

/// @brief Not yet documented.
#define FIX16_MAT33_ROWS      (3)

/// @brief Not yet documented.
#define FIX16_MAT33_ARR_COUNT (FIX16_MAT33_COLUMNS * FIX16_MAT33_ROWS)

/// @cond
typedef union fix16_mat43 fix16_mat43_t;
/// @endcond

/// @brief Not yet documented.
///
/// @note Row-major matrix.
typedef union fix16_mat33 {
    /// @brief Not yet documented.
    struct {
        /// @brief Not yet documented. Row 0.
        fix16_t m00, m01, m02;
        /// @brief Not yet documented. Row 1.
        fix16_t m10, m11, m12;
        /// @brief Not yet documented. Row 2.
        fix16_t m20, m21, m22;
    } comp;

    /// @brief Not yet documented.
    fix16_t arr[FIX16_MAT33_ARR_COUNT];
    /// @brief Not yet documented.
    fix16_t frow[FIX16_MAT33_COLUMNS][FIX16_MAT33_ROWS];
    /// @brief Not yet documented.
    fix16_vec3_t row[FIX16_MAT33_ROWS];
} __packed __aligned(4) fix16_mat33_t;

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat33_dup(const fix16_mat33_t *m0, fix16_mat33_t *result);

/// @brief Not yet documented.
///
/// @param m0 Not yet documented.
extern void fix16_mat33_identity(fix16_mat33_t *m0);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      m1     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat33_mul(const fix16_mat33_t *m0, const fix16_mat33_t *m1, fix16_mat33_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      v      Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat33_vec3_mul(const fix16_mat33_t *m0, const fix16_vec3_t *v, fix16_vec3_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat33_transpose(const fix16_mat33_t * __restrict m0,
  fix16_mat33_t * __restrict result);

/// @brief Not yet documented.
///
/// @param      m0       Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documunted.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_mat33_str(const fix16_mat33_t *m0, char *buffer, int32_t decimals);

/// @brief Not yet documented.
///
/// @param m0 Not yet documented.
extern void fix16_mat33_zero(fix16_mat33_t *m0);

#endif /* !_YAUL_GAMEMATH_FIX16_MAT33_H_ */

/// @}

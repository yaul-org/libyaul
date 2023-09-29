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
#include <gamemath/fix16/fix16_mat33.h>

/// @addtogroup MATH_FIX16_MATRIX
/// @defgroup MATH_FIX16_MATRIX4X3 4x3
/// @ingroup MATH_FIX16_MATRIX
/// @{

/// @brief Not yet documented.
#define FIX16_MAT43_ROWS    (4)
/// @brief Not yet documented.
#define FIX16_MAT43_COLUMNS (3)

/// @brief Not yet documented.
#define FIX16_MAT43_ARR_COUNT (FIX16_MAT43_COLUMNS * FIX16_MAT43_ROWS)

#if !defined(__cplusplus)
/// @cond
// TODO: Remove this
struct fix16_vec3;
// TODO: Remove this
struct fix16_mat33;

// TODO: Remove this
typedef struct fix16_mat33 fix16_mat33_t;
/// @endcond

/// @brief Not yet documented.
///
/// @note Row-major matrix.
typedef struct fix16_mat43 {
    /// @brief Not yet documented.
    fix16_mat33_t rotation;
    /// @brief Not yet documented.
    fix16_vec3_t translation;
} fix16_mat43_t;
#else
struct fix16_vec3_t;
struct fix16_mat33_t;

struct fix16_mat43_t {
    /// @brief Not yet documented.
    fix16_mat33_t rotation;
    /// @brief Not yet documented.
    fix16_vec3_t translation;

    fix16_mat43_t()                     = default;
    fix16_mat43_t(fix16_mat43_t&&)      = default;
    fix16_mat43_t(const fix16_mat43_t&) = default;

    constexpr fix16_mat43_t(
        // Row 0
        fix16_t m00, fix16_t m01, fix16_t m02,
        // Row 1
        fix16_t m10, fix16_t m11, fix16_t m12,
        // Row 2
        fix16_t m20, fix16_t m21, fix16_t m22,
        // Row 3
        fix16_t m30, fix16_t m31, fix16_t m32);

    fix16_mat43_t(const fix16_vec3_t& row0, const fix16_vec3_t& row1, const fix16_vec3_t& row2, const fix16_vec3_t& row3);

    fix16_mat43_t& operator=(const fix16_mat43_t& other) = default;
    fix16_mat43_t& operator=(fix16_mat43_t&& other)      = default;

    inline fix16_mat43_t operator*(const fix16_mat43_t& other) const;

    static inline constexpr fix16_mat43_t identity();

    static inline constexpr fix16_mat43_t zero();

    static inline constexpr fix16_mat43_t from_double(
        // Row 0
        double m00, double m01, double m02,
        // Row 1
        double m10, double m11, double m12,
        // Row 2
        double m20, double m21, double m22,
        // Row 3
        double m30, double m31, double m32);

    inline void set_identity();

    inline void set_zero();

    inline void invert();

    inline size_t to_string(char* buffer, int32_t decimals = 7) const;

    static inline void create_rotx(const fix16_mat43_t& m0, angle_t x, fix16_mat43_t& result);

    static inline void create_roty(const fix16_mat43_t& m0, angle_t y, fix16_mat43_t& result);

    static inline void create_rotz(const fix16_mat43_t& m0, angle_t z, fix16_mat43_t& result);

    static inline void create_rot(const euler_t& angles, fix16_mat43_t& result);

    static inline fix16_vec3_t transform_vector(const fix16_mat43_t& m0, const fix16_vec3_t& v);

    static inline fix16_vec3_t transform_position(const fix16_mat43_t& m0, const fix16_vec3_t& v);
};

static_assert(sizeof(fix16_mat43_t) == (sizeof(fix16_t) * FIX16_MAT43_ARR_COUNT));
#endif /* !__cplusplus */

__BEGIN_DECLS

/// @brief Not yet documented.
///
/// @param m0 Not yet documented.
extern void fix16_mat43_zero(fix16_mat43_t *m0);

/// @brief Not yet documented.
///
/// @param m0 Not yet documented.
extern void fix16_mat43_identity(fix16_mat43_t *m0);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_dup(const fix16_mat43_t *m0, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// The expectation is that the rotation part of the matrix is orthogonal.
///
/// It's then possible to just transpose the 3x3 rotation matrix within the 4x3
/// matrix and negate the translation vector.
///
/// @param      m0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_invert(const fix16_mat43_t *m0, fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// The expectation is that the rotation part of the matrix is orthogonal.
///
/// It's then possible to just transpose the 3x3 rotation matrix within the 4x3
/// matrix and negate the translation vector.
///
/// @param m0 Not yet documented.
extern void fix16_mat43_inplace_invert(fix16_mat43_t *m0);

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
extern void fix16_mat43_mul(const fix16_mat43_t *m0, const fix16_mat43_t *m1,
  fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      v      Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_pos3_mul(const fix16_mat43_t *m0, const fix16_vec3_t *v,
  fix16_vec3_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      v      Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_vec3_mul(const fix16_mat43_t *m0, const fix16_vec3_t *v,
  fix16_vec3_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      angle  Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_x_rotate(const fix16_mat43_t *m0, angle_t angle,
  fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      angle  Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_y_rotate(const fix16_mat43_t *m0, angle_t angle,
  fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0     Not yet documented.
/// @param      angle  Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_z_rotate(const fix16_mat43_t *m0, angle_t angle,
  fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      rx     Not yet documented.
/// @param      ry     Not yet documented.
/// @param      rz     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_rotation_create(angle_t rx, angle_t ry, angle_t rz,
  fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      r      Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_mat43_rotation_set(const fix16_mat33_t *r,
  fix16_mat43_t *result);

/// @brief Not yet documented.
///
/// @param      m0       Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documunted.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_mat43_str(const fix16_mat43_t *m0, char *buffer,
  int32_t decimals);

__END_DECLS

#ifdef __cplusplus
constexpr fix16_mat43_t::fix16_mat43_t(
    // Row 0
    fix16_t m00, fix16_t m01, fix16_t m02,
    // Row 1
    fix16_t m10, fix16_t m11, fix16_t m12,
    // Row 2
    fix16_t m20, fix16_t m21, fix16_t m22,
    // Row 3
    fix16_t m30, fix16_t m31, fix16_t m32)
    : rotation(// Row 0
               m00, m01, m02,
               // Row 1
               m10, m11, m12,
               // Row 2
               m20, m21, m22),
      translation(m30, m31, m32) {}

inline fix16_mat43_t fix16_mat43_t::operator*(const fix16_mat43_t& other) const {
  fix16_mat43_t result;

  fix16_mat43_mul(this, &other, &result);

  return result;
}

inline constexpr fix16_mat43_t fix16_mat43_t::identity() {
    return fix16_mat43_t{// Row 0
                         1.0_fp, 0.0_fp, 0.0_fp,
                         // Row 1
                         0.0_fp, 1.0_fp, 0.0_fp,
                         // Row 2
                         0.0_fp, 0.0_fp, 1.0_fp,
                         // Row 3
                         0.0_fp, 0.0_fp, 0.0_fp};
}

inline constexpr fix16_mat43_t fix16_mat43_t::zero() {
    return fix16_mat43_t{// Row 0
                         0.0_fp, 0.0_fp, 0.0_fp,
                         // Row 1
                         0.0_fp, 0.0_fp, 0.0_fp,
                         // Row 2
                         0.0_fp, 0.0_fp, 0.0_fp,
                         // Row 3
                         0.0_fp, 0.0_fp, 0.0_fp};
}

inline constexpr fix16_mat43_t fix16_mat43_t::from_double(
    // Row 0
    double m00, double m01, double m02,
    // Row 1
    double m10, double m11, double m12,
    // Row 2
    double m20, double m21, double m22,
    // Row 3
    double m30, double m31, double m32) {
    return fix16_mat43_t{
        // Row 0
        fix16_t::from_double(m00), fix16_t::from_double(m01), fix16_t::from_double(m02),
        // Row 1
        fix16_t::from_double(m10), fix16_t::from_double(m11), fix16_t::from_double(m12),
        // Row 2
        fix16_t::from_double(m20), fix16_t::from_double(m21), fix16_t::from_double(m22),
        // Row 3
        fix16_t::from_double(m30), fix16_t::from_double(m31), fix16_t::from_double(m32)};
}

inline void fix16_mat43_t::set_identity() { fix16_mat43_identity(this); }

inline void fix16_mat43_t::set_zero() { fix16_mat43_zero(this); }

inline void fix16_mat43_t::invert() { fix16_mat43_inplace_invert(this); }

inline size_t fix16_mat43_t::to_string(char* buffer, int32_t decimals) const {
    return fix16_mat43_str(this, buffer, decimals);
}

inline void fix16_mat43_t::create_rotx(const fix16_mat43_t& m0, angle_t x, fix16_mat43_t& result) {
    fix16_mat43_x_rotate(&m0, x, &result);
}

inline void fix16_mat43_t::create_roty(const fix16_mat43_t& m0, angle_t y, fix16_mat43_t& result) {
    fix16_mat43_y_rotate(&m0, y, &result);
}

inline void fix16_mat43_t::create_rotz(const fix16_mat43_t& m0, angle_t z, fix16_mat43_t& result) {
    fix16_mat43_z_rotate(&m0, z, &result);
}

inline void fix16_mat43_t::create_rot(const euler_t& angles, fix16_mat43_t& result) {
    fix16_mat43_rotation_create(angles.pitch, angles.yaw, angles.roll, &result);
}

inline fix16_vec3_t fix16_mat43_t::transform_vector(const fix16_mat43_t& m0, const fix16_vec3_t& v) {
    fix16_vec3_t result;

    fix16_mat43_vec3_mul(&m0, &v, &result);

    return result;
}

inline fix16_vec3_t fix16_mat43_t::transform_position(const fix16_mat43_t& m0, const fix16_vec3_t& v) {
    fix16_vec3_t result;

    fix16_mat43_pos3_mul(&m0, &v, &result);

    return result;
}
#endif /* __cplusplus */

/// @}

#endif /* !_YAUL_GAMEMATH_FIX16_MAT43_H_ */

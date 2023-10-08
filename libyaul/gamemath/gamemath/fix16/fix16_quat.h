/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_QUAT_H_
#define _YAUL_GAMEMATH_FIX16_QUAT_H_

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdint.h>

#include <gamemath/fix16.h>
#include <gamemath/fix16/fix16_vec3.h>
#include <gamemath/angle.h>

/// @addtogroup MATH_FIX16_VECTOR
/// @defgroup MATH_FIX16_QUAT quaternions
/// @ingroup MATH_FIX16_VECTOR
/// @{

#if !defined(__cplusplus)
/// @brief Macro for defininge @p fix16_quat_t constant values.
///
/// @note The argument is evaluated multiple times, and also otherwise you
/// should only use this for constant values. For runtime-conversions, use the
/// inlined functions below.
///
/// @param w The constant value.
/// @param x The constant value.
/// @param y The constant value.
/// @param z The constant value.
#define FIX16_QUAT_INITIALIZER(x, y, z, w)                                     \
{                                                                              \
    {                                                                          \
        FIX16(x),                                                              \
        FIX16(y),                                                              \
        FIX16(z)                                                               \
    },                                                                         \
    FIX16(w)                                                                   \
}
#endif /* !__cplusplus */

#if !defined(__cplusplus)
/// @brief Fixed point Q16.16.
typedef struct {
    fix16_vec3_t comp;
    /// @brief Not yet documented.
    fix16_t w;
} fix16_quat_t;
#else
/// @brief Fixed point Q16.16.
struct fix16_quat_t {
    /// @brief Not yet documented.
    fix16_vec3_t comp;
    /// @brief Not yet documented.
    fix16_t w;

    fix16_quat_t() = default;

    ~fix16_quat_t() = default;

    fix16_quat_t& operator=(const fix16_quat_t& other) = default;
    fix16_quat_t& operator=(fix16_quat_t&& other)      = default;

    inline size_t to_string(char* buffer, int32_t decimals = 7) const;

    ///
    static inline void from_euler(const euler_t& angles, fix16_quat_t& result);

    ///
    static inline constexpr fix16_quat_t from_double(double x, double y, double z, double w);
};

static_assert(sizeof(fix16_quat_t) == 16);
#endif /* !__cplusplus */

__BEGIN_DECLS

/// @brief Convert Euler angles to a quaternion in XYZ order.
///
/// @param      rx     Not yet documented.
/// @param      ry     Not yet documented.
/// @param      rz     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_quat_euler(angle_t rx, angle_t ry, angle_t rz,
  fix16_quat_t *result);

/// @brief Not yet documented.
///
/// @param      q0     Not yet documented.
/// @param      q1     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_quat_mul(const fix16_quat_t *q0, const fix16_quat_t *q1,
  fix16_quat_t *result);

/// @brief Not yet documented.
///
/// @param      q0     Not yet documented.
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_quat_vec3_mul(const fix16_quat_t *q0,
  const fix16_vec3_t *v0, fix16_vec3_t *result);

/// @brief Not yet documented.
///
/// @param      q0       Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documented.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_quat_str(const fix16_quat_t *q0, char *buffer,
  int32_t decimals);

__END_DECLS

#if defined(__cplusplus)
inline size_t fix16_quat_t::to_string(char* buffer, int32_t decimals) const { return fix16_quat_str(this, buffer, decimals); }

inline void fix16_quat_t::from_euler(const euler_t& angles, fix16_quat_t& result) {
    fix16_quat_euler(angles.pitch, angles.yaw, angles.roll, &result);
}

constexpr inline fix16_quat_t fix16_quat_t::from_double(double x, double y, double z, double w) {
    return fix16_quat_t{{fix16_t::from_double(x), fix16_t::from_double(y), fix16_t::from_double(z)}, fix16_t::from_double(w)};
}
#endif /* __cplusplus */

/// @}

#endif /* !_YAUL_GAMEMATH_FIX16_QUAT_H_ */

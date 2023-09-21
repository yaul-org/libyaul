#ifndef _YAUL_GAMEMATH_ANGLE_H_
#define _YAUL_GAMEMATH_ANGLE_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <gamemath/defs.h>
#include <gamemath/fix16.h>

__BEGIN_DECLS

/// @addtogroup MATH
/// @defgroup MATH_ANGLE Angle
/// @ingroup MATH
/// @{

/// @brief Not yet documented.
///
/// @param d Not yet documented.
#define RAD2ANGLE(d) ((angle_t)((65536.0f * (d)) / (2 * M_PI)))

/// @brief Not yet documented.
///
/// @param d Not yet documented.
#define DEG2ANGLE(d) ((angle_t)((65536.0f * (d)) / 360.0f))

/// @brief Not yet documented.
typedef int16_t angle_t;

/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Not yet documented.
static inline fix16_t __always_inline
angle_deg_to(angle_t angle)
{
    return (angle * 360);
}

/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Not yet documented.
static inline fix16_t __always_inline
angle_rad_to(angle_t angle)
{
    return fix16_mul(angle, FIX16_2PI);
}

/// @}

__END_DECLS

#endif /* !_YAUL_GAMEMATH_ANGLE_H_ */

/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_PLANE_H_
#define _YAUL_GAMEMATH_FIX16_PLANE_H_

#include <gamemath/fix16/fix16_vec3.h>

__BEGIN_DECLS

/// @addtogroup MATH_FIX16_PLANES
/// @{

/// @brief Not yet documented.
///
/// @param nx Not yet documented.
/// @param ny Not yet documented.
/// @param nz Not yet documented.
/// @param dx Not yet documented.
/// @param dy Not yet documented.
/// @param dz Not yet documented.
#define FIX16_PLANE_INITIALIZER(nx, ny, nz, dx, dy, dz)                        \
{                                                                              \
    .normal = {                                                                \
        FIX16(nx),                                                             \
        FIX16(ny),                                                             \
        FIX16(nz)                                                              \
    },                                                                         \
    .d = {                                                                     \
        FIX16(dx),                                                             \
        FIX16(dy),                                                             \
        FIX16(dz)                                                              \
    }                                                                          \
}

/// @param ny Not yet documented.
typedef struct fix16_plane {
    /// @param ny Not yet documented.
    fix16_vec3_t normal;
    /// @param ny Not yet documented.
    fix16_vec3_t d;
} fix16_plane_t;

/// @brief Not yet documented.
///
/// @param      plane    Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documunted.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_plane_str(const fix16_plane_t *plane, char *buffer, int32_t decimals);

/// @}

__END_DECLS

#endif /* !_YAUL_GAMEMATH_FIX16_PLANE_H_ */

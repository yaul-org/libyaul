/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_INT8_H_
#define _YAUL_MATH_INT8_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @addtogroup MATH
/// @defgroup MATH_INT8 Int8
/// @ingroup MATH
/// @{

/// @brief 2-vector of type @p int8_t.
typedef struct {
    /// @brief _X_ component.
    int8_t x;
    /// @brief _Y_ component.
    int8_t y;
} int8_vec2_t;

/// @brief 3-vector of type @p int8_t.
typedef struct {
    /// @brief _X_ component.
    int8_t x;
    /// @brief _Y_ component.
    int8_t y;
    /// @brief _Z_ component.
    int8_t z;
} int8_vec3_t;

/// @brief 2-vector initializer.
///
/// @param x The _X_ component.
/// @param y The _Y_ component.
#define INT8_VEC2_INITIALIZER(x, y)                                            \
{                                                                              \
    (x),                                                                       \
    (y)                                                                        \
}

/// @brief 2-vector.
///
/// @param x The _X_ component.
/// @param y The _Y_ component.
#define INT8_VEC2(x, y)                                                        \
((int8_vec2_t){                                                                \
    (x),                                                                       \
    (y)                                                                        \
})

/// @brief 3-vector initializer.
///
/// @param x The _X_ component.
/// @param y The _Y_ component.
/// @param z The _Z_ component.
#define INT8_VEC3_INITIALIZER(x, y, z)                                         \
{                                                                              \
    (x),                                                                       \
    (y),                                                                       \
    (z)                                                                        \
}

/// @brief 3-vector.
///
/// @param x The _X_ component.
/// @param y The _Y_ component.
/// @param z The _Z_ component.
#define INT8_VEC3(x, y, z)                                                     \
((int8_vec3_t){                                                                \
    (x),                                                                       \
    (y),                                                                       \
    (z)                                                                        \
})

/// @}

__END_DECLS

#endif /* !_YAUL_MATH_INT8_H_ */

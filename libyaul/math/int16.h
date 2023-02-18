/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_INT16_H_
#define _YAUL_MATH_INT16_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @addtogroup MATH
/// @defgroup MATH_INT16 Int16
/// @ingroup MATH
/// @{

/// @brief 2-vector of type @p int16_t.
typedef struct {
    /// @brief _X_ component.
    int16_t x;
    /// @brief _Y_ component.
    int16_t y;
} __aligned(4) int16_vec2_t;

/// @brief 2-vector initializer.
///
/// @param x The _X_ component.
/// @param y The _Y_ component.
#define INT16_VEC2_INITIALIZER(x, y)                                           \
{                                                                              \
    (x),                                                                       \
    (y)                                                                        \
}

/// @brief Not yet documented.
///
/// @param[out] result Not yet documented.
static inline void __always_inline
int16_vec2_zero(int16_vec2_t *result)
{
    result->x = 0;
    result->y = 0;
}

/// @brief Not yet documented.
///
/// @param[in]  v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
int16_vec2_dup(const int16_vec2_t * __restrict v0,
  int16_vec2_t * __restrict result)
{
    result->x = v0->x;
    result->y = v0->y;
}

/// @brief Not yet documented.
///
/// @param[in]  v0     Not yet documented.
/// @param[in]  v1     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
int16_vec2_add(const int16_vec2_t * __restrict v0,
  const int16_vec2_t * __restrict v1,
  int16_vec2_t * __restrict result)
{
    result->x = v0->x + v1->x;
    result->y = v0->y + v1->y;
}

/// @brief Not yet documented.
///
/// @param[in]  v0     Not yet documented.
/// @param[in]  v1     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
int16_vec2_sub(const int16_vec2_t * __restrict v0,
  const int16_vec2_t * __restrict v1,
  int16_vec2_t * __restrict result)
{
    result->x = v0->x - v1->x;
    result->y = v0->y - v1->y;
}

/// @brief Not yet documented.
///
/// @param      scalar Not yet documented.
/// @param[in]  v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
int16_vec2_scale(const int8_t scalar, const int16_vec2_t * __restrict v0,
  int16_vec2_t * __restrict result)
{
    result->x = scalar * v0->x;
    result->y = scalar * v0->y;
}

/// @brief Not yet documented.
///
/// @param scalar Not yet documented.
/// @param in_out Not yet documented.
static inline void __always_inline
int16_vec2_scaled(const int8_t scalar, int16_vec2_t *in_out)
{
    in_out->x = scalar * in_out->x;
    in_out->y = scalar * in_out->y;
}

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param[out] buffer Not yet documented.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t int16_vec2_str(const int16_vec2_t *v0, char *buffer);

/// @}

__END_DECLS

#endif /* !_YAUL_MATH_INT16_H_ */

/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FLOAT_H_
#define _YAUL_GAMEMATH_FLOAT_H_

#include <sys/cdefs.h>

#include <stdint.h>

/// @defgroup MATH_FLOAT Float
/// @ingroup MATH

#if defined(__cplusplus)
constexpr static inline double fmod(double x, double y) { return (x - static_cast<int>(x / y) * y); }

constexpr static inline float fmodf(float x, float y) { return (x - static_cast<int>(x / y) * y); }
#endif /* !__cplusplus */

#endif /* !_YAUL_GAMEMATH_FLOAT_H_ */

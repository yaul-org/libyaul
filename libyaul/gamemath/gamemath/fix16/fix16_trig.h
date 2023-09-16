/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_TRIG_H_
#define _YAUL_GAMEMATH_FIX16_TRIG_H_

#include <gamemath/angle.h>
#include <gamemath/fix16.h>

__BEGIN_DECLS

/// @addtogroup MATH_FIX16_TRIG
/// @{

/// @brief Not yet documented.
#define FIX16_LUT_SIN_TABLE_SHIFT  (10)

/// @brief Not yet documented.
#define FIX16_LUT_SIN_TABLE_COUNT  (1024)

/// @brief Not yet documented.
#define FIX16_LUT_ATAN_TABLE_SHIFT (8)

/// @brief Not yet documented.
#define FIX16_LUT_ATAN_TABLE_COUNT (256)

/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Value.
extern fix16_t fix16_sin(angle_t angle);

/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Value.
extern fix16_t fix16_cos(angle_t angle);

/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Value.
extern fix16_t fix16_tan(angle_t angle);

/// @brief Not yet documented.
///
/// @param      angle      Not yet documented.
/// @param[out] result_sin Not yet documented.
/// @param[out] result_cos Not yet documented.
extern void fix16_sincos(angle_t angle, fix16_t *result_sin, fix16_t *result_cos);

/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
///
/// @returns Value.
extern angle_t fix16_atan2(fix16_t y, fix16_t x);

/// @}

__END_DECLS

#endif /* !_YAUL_GAMEMATH_FIX16_TRIG_H_ */

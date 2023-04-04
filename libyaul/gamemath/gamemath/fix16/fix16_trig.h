/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_H_
#error "Header file must not be directly included"
#endif /* !_YAUL_GAMEMATH_FIX16_H_ */

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
extern fix16_t fix16_sin(angle_t angle) FIXMATH_FUNC_ATTRS;

/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Value.
extern fix16_t fix16_cos(angle_t angle) FIXMATH_FUNC_ATTRS;

/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Value.
extern fix16_t fix16_tan(angle_t angle) FIXMATH_FUNC_ATTRS;

/// @brief Not yet documented.
///
/// @param      angle      Not yet documented.
/// @param[out] result_sin Not yet documented.
/// @param[out] result_cos Not yet documented.
extern void fix16_sincos(angle_t angle, fix16_t *result_sin, fix16_t *result_cos) FIXMATH_FUNC_NONCONST_ATTRS;

/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
///
/// @returns Value.
extern angle_t fix16_atan2(fix16_t y, fix16_t x) FIXMATH_FUNC_ATTRS;

/// @}

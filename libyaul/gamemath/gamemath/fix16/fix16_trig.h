/*
 * Copyright (c)
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_TRIG_H_
#define _YAUL_GAMEMATH_FIX16_TRIG_H_

#include <gamemath/angle.h>
#include <gamemath/fix16.h>

__BEGIN_DECLS

/// @addtogroup MATH_FIX16_TRIG
/// @{

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
extern void fix16_sincos(angle_t angle, fix16_t *result_sin,
    fix16_t *result_cos);

/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
///
/// @returns Value.
extern angle_t fix16_atan2(fix16_t y, fix16_t x);

__END_DECLS

#if defined(__cplusplus)
static inline fix16_t cos(angle_t angle) { return fix16_cos(angle); }

static inline fix16_t sin(angle_t angle) { return fix16_sin(angle); }

static inline fix16_t tan(angle_t angle) { return fix16_tan(angle); }

static inline void sincos(angle_t angle, fix16_t& out_sin, fix16_t& out_cos) {
    fix16_sincos(angle, &out_sin, &out_cos);
}
#endif /* __cplusplus */

/// @}

#endif /* !_YAUL_GAMEMATH_FIX16_TRIG_H_ */

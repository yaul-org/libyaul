/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_FIX16_H_
#error "Header file must not be directly included"
#endif /* !_YAUL_MATH_FIX16_H_ */

#define FIX16_LUT_SIN_TABLE_SHIFT  (10)
#define FIX16_LUT_SIN_TABLE_COUNT  (1024)
#define FIX16_LUT_ATAN_TABLE_SHIFT (8)
#define FIX16_LUT_ATAN_TABLE_COUNT (256)

extern fix16_t fix16_sin(angle_t angle) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_cos(angle_t angle) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_tan(angle_t angle) FIXMATH_FUNC_ATTRS;
extern void fix16_sincos(angle_t angle, fix16_t *result_sin, fix16_t *result_cos) FIXMATH_FUNC_NONCONST_ATTRS;

extern angle_t fix16_atan2(fix16_t y, fix16_t x) FIXMATH_FUNC_ATTRS;

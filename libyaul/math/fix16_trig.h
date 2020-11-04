/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _FIX16_INCLUDE_ONCE
#error "Header file must not be directly included"
#endif /* !_FIX16_INCLUDE_ONCE */

#define FIX16_LUT_SIN_TABLE_COUNT       (1024)
#define FIX16_LUT_ATAN_TABLE_COUNT      (256)

extern fix16_t fix16_sin(fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_cos(fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_tan(fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_atan2(fix16_t y, fix16_t x) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_bradians_sin(int32_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_bradians_cos(int32_t) FIXMATH_FUNC_ATTRS;

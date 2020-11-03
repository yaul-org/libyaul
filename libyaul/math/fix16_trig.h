/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _FIX16_INCLUDE_ONCE
#error "Header file must not be directly included"
#endif /* !_FIX16_INCLUDE_ONCE */

#define FIX16_LUT_TABLE_COUNT (1024)

extern fix16_t fix16_sin(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_cos(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_tan(const fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_bradians_sin(const int32_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_bradians_cos(const int32_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_asin(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_acos(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_atan(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_atan2(const fix16_t, const fix16_t) FIXMATH_FUNC_ATTRS;

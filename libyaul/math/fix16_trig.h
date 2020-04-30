/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _FIX16_INCLUDE_ONCE
#error "Header file must not be directly included"
#endif /* !_FIX16_INCLUDE_ONCE */

typedef struct fix16_sin_table {
        const fix16_t *table;
} fix16_sin_table_t;

typedef struct fix16_cos_table {
        const fix16_t *table;
} fix16_cos_table_t;

typedef struct fix16_atan2_table {
        const fix16_t *table;
} fix16_atan2_table_t;

extern void fix16_trig_tables_set(const fix16_sin_table_t *,
    const fix16_cos_table_t *, const fix16_atan2_table_t *);

extern fix16_t fix16_sin(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_cos(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_tan(const fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_asin(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_acos(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_atan(const fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_atan2(const fix16_t, const fix16_t) FIXMATH_FUNC_ATTRS;

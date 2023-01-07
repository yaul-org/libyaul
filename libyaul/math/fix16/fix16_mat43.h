/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _YAUL_MATH_FIX16_H_
#error "Header file must not be directly included"
#endif /* !_YAUL_MATH_FIX16_H_ */

#define FIX16_MAT43_COLUMNS   (4)
#define FIX16_MAT43_ROWS      (3)
#define FIX16_MAT43_ARR_COUNT (FIX16_MAT43_COLUMNS * FIX16_MAT43_ROWS)

union fix16_vec3;
union fix16_mat33;

typedef union fix16_vec3 fix16_vec3_t;
typedef union fix16_mat33 fix16_mat33_t;

/* Row-major matrix */
typedef union fix16_mat43 {
        struct {
                fix16_t m00, m01, m02, m03; /* Row 0 */
                fix16_t m10, m11, m12, m13; /* Row 1 */
                fix16_t m20, m21, m22, m33; /* Row 2 */
        } comp;

        fix16_t arr[FIX16_MAT43_ARR_COUNT];
        fix16_t frow[FIX16_MAT43_ROWS][FIX16_MAT43_COLUMNS];
        fix16_vec4_t row[FIX16_MAT43_ROWS];
} __aligned(4) fix16_mat43_t;

static inline void __always_inline
fix16_mat43_translation_set(fix16_mat43_t *m0, const fix16_vec3_t *t)
{
        m0->frow[0][3] = t->x;
        m0->frow[1][3] = t->y;
        m0->frow[2][3] = t->z;
}

static inline void __always_inline
fix16_mat43_x_translate(const fix16_mat43_t *m0, fix16_mat43_t *result, fix16_t x)
{
        result->frow[0][3] = m0->frow[0][3] + x;
}

static inline void __always_inline
fix16_mat43_y_translate(const fix16_mat43_t *m0, fix16_mat43_t *result, fix16_t y)
{
        result->frow[1][3] = m0->frow[1][3] + y;
}

static inline void __always_inline
fix16_mat43_z_translate(const fix16_mat43_t *m0, fix16_mat43_t *result, fix16_t z)
{
        result->frow[2][3] = m0->frow[2][3] + z;
}

static inline void __always_inline
fix16_mat43_translate(const fix16_mat43_t *m0, fix16_mat43_t *result, const fix16_vec3_t *t)
{
        fix16_mat43_x_translate(m0, result, t->x);
        fix16_mat43_y_translate(m0, result, t->y);
        fix16_mat43_z_translate(m0, result, t->z);
}

extern void fix16_mat43_dup(const fix16_mat43_t *m0, fix16_mat43_t *result);
extern void fix16_mat43_invert(const fix16_mat43_t *m0, fix16_mat43_t *result);
extern void fix16_mat43_mul(const fix16_mat43_t *m0, const fix16_mat43_t *m1, fix16_mat43_t *result);
extern void fix16_mat43_x_rotate(const fix16_mat43_t *m0, fix16_mat43_t *result, angle_t angle);
extern void fix16_mat43_y_rotate(const fix16_mat43_t *m0, fix16_mat43_t *result, angle_t angle);
extern void fix16_mat43_z_rotate(const fix16_mat43_t *m0, fix16_mat43_t *result, angle_t angle);
extern void fix16_mat43_rotation_create(fix16_mat43_t *m0, angle_t rx, angle_t ry, angle_t rz);
extern void fix16_mat43_rotation_set(const fix16_mat33_t *r, fix16_mat43_t *result);
extern void fix16_mat43_identity(fix16_mat43_t *m0);
extern size_t fix16_mat43_str(const fix16_mat43_t *m0, char *buffer, int32_t decimals);
extern void fix16_mat43_zero(fix16_mat43_t *m0);

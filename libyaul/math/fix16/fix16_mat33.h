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

#define FIX16_MAT33_COLUMNS   (3)
#define FIX16_MAT33_ROWS      (3)
#define FIX16_MAT33_ARR_COUNT (FIX16_MAT33_COLUMNS * FIX16_MAT33_ROWS)

union fix16_vec3;

typedef union fix16_vec3 fix16_vec3_t;

/* Row-major matrix */
typedef union fix16_mat33 {
        struct {
                fix16_t m00, m01, m02; /* Row 0 */
                fix16_t m10, m11, m12; /* Row 1 */
                fix16_t m20, m21, m22; /* Row 2 */
        } comp;

        fix16_t arr[FIX16_MAT33_ARR_COUNT];
        fix16_t frow[FIX16_MAT33_COLUMNS][FIX16_MAT33_ROWS];
        fix16_vec3_t row[FIX16_MAT33_ROWS];
} __aligned(4) fix16_mat33_t;

extern void fix16_mat33_dup(const fix16_mat33_t *m0, fix16_mat33_t *result);
extern void fix16_mat33_identity(fix16_mat33_t *m0);
extern size_t fix16_mat33_str(const fix16_mat33_t *m0, char *buffer, int32_t decimals);
extern void fix16_mat33_transpose(const fix16_mat33_t * __restrict m0,
    fix16_mat33_t * __restrict result);
extern void fix16_mat33_zero(fix16_mat33_t *m0);

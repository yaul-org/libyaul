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

union fix16_vec3;

typedef union fix16_vec3 fix16_vec3_t;

/* Row-major matrix */
typedef union fix16_mat {
        struct {
                fix16_t m00, m01, m02, m03; /* Row 0 */
                fix16_t m10, m11, m12, m13; /* Row 1 */
                fix16_t m20, m21, m22, m33; /* Row 2 */
        } comp;

        fix16_t arr[12];
        fix16_t frow[3][4];
        fix16_vec4_t row[3];
} __aligned(4) fix16_mat_t;

extern void fix16_mat_dup(const fix16_mat_t *m0, fix16_mat_t *result);
extern void fix16_mat_identity(fix16_mat_t *m0);
extern size_t fix16_mat_str(const fix16_mat_t *m0, char *buffer, int32_t decimals);
extern void fix16_mat_zero(fix16_mat_t *m0);

/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _FIX16_INCLUDE_ONCE
#error "Header file must not be directly included"
#endif /* !_FIX16_INCLUDE_ONCE */

/* Row-major matrix */
typedef union fix16_mat4 {
        struct {
                fix16_t m00, m01, m02, m03; /* Row 0 */
                fix16_t m10, m11, m12, m13; /* Row 1 */
                fix16_t m20, m21, m22, m23; /* Row 2 */
                fix16_t m30, m31, m32, m33; /* Row 3 */
        } comp;

        fix16_t arr[16];
        fix16_t frow[4][4];
        fix16_vec4_t row[4];
} __aligned(64) fix16_mat4_t;

extern void fix16_mat4_dup(const fix16_mat4_t * __restrict,
    fix16_mat4_t * __restrict);
extern void fix16_mat4_identity(fix16_mat4_t *);
extern void fix16_mat4_str(const fix16_mat4_t *, char *, int);
extern void fix16_mat4_transpose(const fix16_mat4_t * __restrict,
    fix16_mat4_t * __restrict);
extern void fix16_mat4_zero(fix16_mat4_t *);

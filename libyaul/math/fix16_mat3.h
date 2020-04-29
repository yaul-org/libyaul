#ifndef _MATH_FIX16_MAT3_H_
#define _MATH_FIX16_MAT3_H_

union fix16_vec3;

typedef union fix16_vec3 fix16_vec3_t;

/* Row-major matrix */
typedef union fix16_mat3 {
        struct {
                fix16_t m00, m01, m02; /* Row 0 */
                fix16_t m10, m11, m12; /* Row 1 */
                fix16_t m20, m21, m22; /* Row 2 */
        } comp;

        fix16_t arr[9];
        fix16_t frow[3][3];
        fix16_vec3_t row[3];
} __aligned(64) fix16_mat3_t;

extern void fix16_mat3_dup(const fix16_mat3_t *, fix16_mat3_t *);
extern void fix16_mat3_identity(fix16_mat3_t *);
extern void fix16_mat3_str(const fix16_mat3_t *, char *, int);
extern void fix16_mat3_transpose(const fix16_mat3_t * __restrict,
    fix16_mat3_t * __restrict);
extern void fix16_mat3_zero(fix16_mat3_t *);

#endif /* _MATH_FIX16_MAT3_H_ */

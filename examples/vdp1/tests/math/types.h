/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef TYPES_H
#define TYPES_H

#include <fixmath.h>

#define __unused__      __attribute__ ((unused))
#define __packed__      __attribute__ ((packed))
#define __aligned__(x)  __attribute__ ((aligned(x)))

typedef struct {
        int16_t x;
        int16_t y;
} int16_vector2_t __aligned__(4);

typedef struct {
        fix16_t x;
        fix16_t y;
        fix16_t z;
} fix16_vector3_t __aligned__(16);

typedef struct {
        fix16_t x;
        fix16_t y;
        fix16_t z;
        fix16_t w;
} fix16_vector4_t __aligned__(16);

/* Row-major matrix */
typedef union {
        struct {
                fix16_t m00, m01, m02, m03; /* Row 0 */
                fix16_t m10, m11, m12, m13; /* Row 1 */
                fix16_t m20, m21, m22, m23; /* Row 2 */
                fix16_t m30, m31, m32, m33; /* Row 3 */
        } comp __aligned__(32);

        fix16_t arr[16] __aligned__(32);
        fix16_t frow[4][4] __aligned__(32);
        fix16_vector4_t row[4] __aligned__(32);
} fix16_matrix4_t;

#endif /* !TYPES_H */

#ifndef __libfixmath_int16_h__
#define __libfixmath_int16_h__

#include <stdint.h>

#ifndef __aligned
#define __aligned(x)    __attribute__ ((aligned(x)))
#endif /* !__aligned */

typedef struct {
        int16_t x;
        int16_t y;
} __aligned(4) int16_vector2_t;

#define INT16_VECTOR2_INITIALIZER(x, y)                                        \
    {                                                                          \
            (x),                                                               \
            (y)                                                                \
    }

extern int16_t int16_vector2_dot(const int16_vector2_t *,
    const int16_vector2_t *);
extern void int16_vector2_add(const int16_vector2_t *, const int16_vector2_t *,
    int16_vector2_t *);
extern void int16_vector2_dup(const int16_vector2_t *, int16_vector2_t *);
extern void int16_vector2_scale(int8_t, int16_vector2_t *);
extern void int16_vector2_scaled(int8_t, const int16_vector2_t *,
    int16_vector2_t *);
extern void int16_vector2_str(const int16_vector2_t *, char *);
extern void int16_vector2_sub(const int16_vector2_t *, const int16_vector2_t *,
    int16_vector2_t *);
extern void int16_vector2_zero(int16_vector2_t *);

#endif /* !__libfixmath_int16_h__ */

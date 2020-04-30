/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _MATH_FIX16_VEC2_H_
#define _MATH_FIX16_VEC2_H_

#define FIX16_VEC2_INITIALIZER(x, y)                                           \
    {                                                                          \
            {                                                                  \
                    FIX16(x),                                                  \
                    FIX16(y)                                                   \
            }                                                                  \
    }

typedef union fix16_vec2 {
        struct {
                fix16_t x;
                fix16_t y;
        };

        fix16_t comp[2];
} __packed __aligned(4) fix16_vec2_t;

static inline void __always_inline
fix16_vec2_zero(fix16_vec2_t *result)
{
        result->x = FIX16(0.0f);
        result->y = FIX16(0.0f);
}

static inline void __always_inline
fix16_vec2_dup(const fix16_vec2_t * __restrict v0,
    fix16_vec2_t * __restrict result)
{
        result->x = v0->x;
        result->y = v0->y;
}

static inline void __always_inline
fix16_vec2_add(const fix16_vec2_t * __restrict v0,
    const fix16_vec2_t * __restrict v1, fix16_vec2_t * __restrict result)
{
        result->x = v0->x + v1->x;
        result->y = v0->y + v1->y;
}

static inline void __always_inline
fix16_vec2_sub(const fix16_vec2_t * __restrict v0,
    const fix16_vec2_t * __restrict v1, fix16_vec2_t * __restrict result)
{
        result->x = v0->x - v1->x;
        result->y = v0->y - v1->y;
}

static inline void __always_inline
fix16_vec2_scale(const fix16_t scalar, fix16_vec2_t *result)
{
        result->x = scalar * result->x;
        result->y = scalar * result->y;
}

static inline void __always_inline
fix16_vec2_scaled(const fix16_t scalar, const fix16_vec2_t * __restrict v0,
    fix16_vec2_t * __restrict result)
{
        result->x = scalar * v0->x;
        result->y = scalar * v0->y;
}

static inline fix16_t __always_inline
fix16_vec2_inline_dot(const fix16_vec2_t *a, const fix16_vec2_t *b)
{
        /* We need to copy a and b parameters because the registers addresses
         * will be modified by mac.l after every instruction. */
        register uint32_t aux0;
        register uint32_t aux1;
        register uint32_t aux2;
        register uint32_t aux3;

        __asm__ volatile ("\tclrmac\n"
                          "\tmov %[a], %[aux0]\n"
                          "\tmov %[b], %[aux1]\n"
                          "\tmac.l @%[aux0]+, @%[aux1]+\n"
                          "\tmac.l @%[aux0]+, @%[aux1]+\n"
                          "\tsts mach, %[aux2]\n"
                          "\tsts macl, %[aux3]\n"
                          "\txtrct %[aux2], %[aux3]\n"
            : [aux0] "=&r" (aux0),
              [aux1] "=&r" (aux1),
              [aux2] "=&r" (aux2),
              [aux3] "=&r" (aux3)
            : [a] "r" (a),
              [b] "r" (b));

        return aux3;
}

extern fix16_t fix16_vec2_length(const fix16_vec2_t *);
extern fix16_t fix16_vec2_sqr_length(const fix16_vec2_t *);
extern void fix16_vec2_normalize(fix16_vec2_t *);
extern void fix16_vec2_normalized(const fix16_vec2_t *, fix16_vec2_t *);
extern fix16_t fix16_vec2_dot(const fix16_vec2_t *, const fix16_vec2_t *);
extern void fix16_vec2_str(const fix16_vec2_t *, char *, int);

#endif /* _MATH_FIX16_VEC2_H_ */

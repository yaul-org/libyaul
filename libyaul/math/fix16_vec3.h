/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _MATH_FIX16_VEC3_H_
#define _MATH_FIX16_VEC3_H_

#define FIX16_VEC3_INITIALIZER(x, y, z)                                        \
    {                                                                          \
            {                                                                  \
                    FIX16(x),                                                  \
                    FIX16(y),                                                  \
                    FIX16(z)                                                   \
            }                                                                  \
    }

typedef union fix16_vec3 {
        struct {
                fix16_t x;
                fix16_t y;
                fix16_t z;
        };

        fix16_t comp[3];
} __packed __aligned(4) fix16_vec3_t;

static inline void __always_inline
fix16_vec3_zero(fix16_vec3_t *result)
{
        result->x = FIX16(0.0f);
        result->y = FIX16(0.0f);
        result->z = FIX16(0.0f);
}

static inline void __always_inline
fix16_vec3_dup(const fix16_vec3_t * __restrict v0,
    fix16_vec3_t * __restrict result)
{
        result->x = v0->x;
        result->y = v0->y;
        result->z = v0->z;
}

static inline void __always_inline
fix16_vec3_add(const fix16_vec3_t * __restrict v0,
    const fix16_vec3_t * __restrict v1, fix16_vec3_t * __restrict result)
{
        result->x = v0->x + v1->x;
        result->y = v0->y + v1->y;
        result->z = v0->z + v1->z;
}

static inline void __always_inline
fix16_vec3_sub(const fix16_vec3_t * __restrict v0,
    const fix16_vec3_t * __restrict v1, fix16_vec3_t * __restrict result)
{
        result->x = v0->x - v1->x;
        result->y = v0->y - v1->y;
        result->z = v0->z - v1->z;
}

static inline void __always_inline
fix16_vec3_scale(const fix16_t scalar, fix16_vec3_t *result)
{
        result->x = scalar * result->x;
        result->y = scalar * result->y;
        result->z = scalar * result->z;
}

static inline void __always_inline
fix16_vec3_scaled(const fix16_t scalar, const fix16_vec3_t * __restrict v,
    fix16_vec3_t * __restrict result)
{
        result->x = scalar * v->x;
        result->y = scalar * v->y;
        result->z = scalar * v->z;
}

static inline fix16_t __always_inline
fix16_vec3_inline_dot(const fix16_vec3_t *a, const fix16_vec3_t *b)
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

extern fix16_t fix16_vec3_length(const fix16_vec3_t *);
extern void fix16_vec3_dup(const fix16_vec3_t * __restrict, fix16_vec3_t * __restrict);
extern void fix16_vec3_normalize(fix16_vec3_t *);
extern void fix16_vec3_normalized(const fix16_vec3_t * __restrict,
    fix16_vec3_t * __restrict);
extern fix16_t fix16_vec3_dot(const fix16_vec3_t *, const fix16_vec3_t *);
extern void fix16_vec3_str(const fix16_vec3_t *, char *, int);

#endif /* _MATH_FIX16_VEC3_H_ */

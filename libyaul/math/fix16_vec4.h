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

#define FIX16_VEC4_INITIALIZER(x, y, z, w)                                     \
    {                                                                          \
            FIX16(x),                                                          \
            FIX16(y),                                                          \
            FIX16(z),                                                          \
            FIX16(w),                                                          \
    }

typedef union fix16_vec4 {
        struct {
                fix16_t x;
                fix16_t y;
                fix16_t z;
                fix16_t w;
        };

        fix16_t comp[4];
} __packed __aligned(16) fix16_vec4_t;

static inline void
fix16_vec4_zero(fix16_vec4_t *result)
{
        result->x = FIX16(0.0f);
        result->y = FIX16(0.0f);
        result->z = FIX16(0.0f);
        result->w = FIX16_ONE;
}

static inline void
fix16_vec4_dup(const fix16_vec4_t * __restrict v0,
    fix16_vec4_t * __restrict result)
{
        result->x = v0->x;
        result->y = v0->y;
        result->z = v0->z;
        result->w = v0->w;
}

static inline void
fix16_vec4_add(const fix16_vec4_t * __restrict v0,
    const fix16_vec4_t * __restrict v1, fix16_vec4_t * __restrict result)
{
        result->x = v0->x + v1->x;
        result->y = v0->y + v1->y;
        result->z = v0->z + v1->z;
        result->w = FIX16_ONE;
}

static inline void
fix16_vec4_sub(const fix16_vec4_t * __restrict v0,
    const fix16_vec4_t * __restrict v1, fix16_vec4_t * __restrict result)
{
        result->x = v0->x - v1->x;
        result->y = v0->y - v1->y;
        result->z = v0->z - v1->z;
        result->w = FIX16_ONE;
}

static inline void
fix16_vec4_scale(const fix16_t scalar, fix16_vec4_t *result)
{
        result->x = fix16_mul(scalar, result->x);
        result->y = fix16_mul(scalar, result->y);
        result->z = fix16_mul(scalar, result->z);
        result->w = FIX16_ONE;
}

static inline void
fix16_vec4_scaled(const fix16_t scalar, const fix16_vec4_t * __restrict v0,
    fix16_vec4_t * __restrict result)
{
        result->x = fix16_mul(scalar, v0->x);
        result->y = fix16_mul(scalar, v0->y);
        result->z = fix16_mul(scalar, v0->z);
        result->w = FIX16_ONE;
}

static inline fix16_t __always_inline
fix16_vec4_inline_dot(const fix16_vec4_t *a, const fix16_vec4_t *b)
{
        register uint32_t aux0;
        register uint32_t aux1;

        __asm__ volatile ("\tclrmac\n"
                          "\tmac.l @%[a]+, @%[b]+\n"
                          "\tmac.l @%[a]+, @%[b]+\n"
                          "\tmac.l @%[a]+, @%[b]+\n"
                          "\tmac.l @%[a]+, @%[b]+\n"
                          "\tsts mach, %[aux0]\n"
                          "\tsts macl, %[aux1]\n"
                          "\txtrct %[aux0], %[aux1]\n"
            : [a] "+r" (a),
              [b] "+r" (b),
              [aux0] "=&r" (aux0),
              [aux1] "=&r" (aux1)
            : "m" (*a),
              "m" (*b)
            : "mach", "macl", "memory");

        return aux1;
}

extern fix16_t fix16_vec4_length(const fix16_vec4_t *);
extern fix16_t fix16_vec4_sqr_length(const fix16_vec2_t *);
extern void fix16_vec4_normalize(fix16_vec4_t *);
extern void fix16_vec4_normalized(const fix16_vec4_t * __restrict,
    fix16_vec4_t * __restrict);
extern fix16_t fix16_vec4_dot(const fix16_vec4_t *, const fix16_vec4_t *);
extern void fix16_vec4_str(const fix16_vec4_t *, char *, uint8_t);

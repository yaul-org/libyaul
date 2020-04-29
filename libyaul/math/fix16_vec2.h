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

extern fix16_t fix16_vec2_length(const fix16_vec2_t *);
extern void fix16_vec2_normalize(fix16_vec2_t *);
extern void fix16_vec2_normalized(const fix16_vec2_t *, fix16_vec2_t *);
extern void fix16_vec2_str(const fix16_vec2_t *, char *, int);

#endif /* _MATH_FIX16_VEC2_H_ */

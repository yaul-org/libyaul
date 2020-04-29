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

extern fix16_t fix16_vec3_length(const fix16_vec3_t *);
extern void fix16_vec3_dup(const fix16_vec3_t * __restrict, fix16_vec3_t * __restrict);
extern void fix16_vec3_normalize(fix16_vec3_t *);
extern void fix16_vec3_normalized(const fix16_vec3_t * __restrict,
    fix16_vec3_t * __restrict);
extern void fix16_vec3_str(const fix16_vec3_t *, char *, int);

#endif /* _MATH_FIX16_VEC3_H_ */

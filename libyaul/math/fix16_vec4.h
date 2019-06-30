#define FIX16_VEC4_INITIALIZER(x, y, z, w)                                     \
    {                                                                          \
            FIX16(x),                                                          \
            FIX16(y),                                                          \
            FIX16(z),                                                          \
            FIX16(w),                                                          \
    }

typedef union {
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
    const fix16_vec4_t * __restrict v1,
    fix16_vec4_t * __restrict result)
{
        result->x = v0->x + v1->x;
        result->y = v0->y + v1->y;
        result->z = v0->z + v1->z;
        result->w = FIX16_ONE;
}

static inline void
fix16_vec4_sub(const fix16_vec4_t * __restrict v0,
    const fix16_vec4_t * __restrict v1,
    fix16_vec4_t * __restrict result)
{
        result->x = v0->x - v1->x;
        result->y = v0->y - v1->y;
        result->z = v0->z - v1->z;
        result->w = FIX16_ONE;
}

static inline void
fix16_vec4_scale(const fix16_t scalar, fix16_vec4_t *result)
{
        result->x = scalar * result->x;
        result->y = scalar * result->y;
        result->z = scalar * result->z;
        result->w = FIX16_ONE;
}

static inline void
fix16_vec4_scaled(const fix16_t scalar,
    const fix16_vec4_t * __restrict v0,
    fix16_vec4_t * __restrict result)
{
        result->x = scalar * v0->x;
        result->y = scalar * v0->y;
        result->z = scalar * v0->z;
        result->w = FIX16_ONE;
}

extern fix16_t fix16_vec4_length(const fix16_vec4_t *);
extern void fix16_vec4_cross(const fix16_vec4_t * __restrict, const fix16_vec4_t * __restrict, fix16_vec4_t * __restrict);
extern void fix16_vec4_normalize(fix16_vec4_t *);
extern void fix16_vec4_normalized(const fix16_vec4_t * __restrict, fix16_vec4_t * __restrict);
extern void fix16_vec4_str(const fix16_vec4_t *, char *, uint8_t);

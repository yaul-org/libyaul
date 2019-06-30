#define FIX16_VEC2_INITIALIZER(x, y)                                           \
    {                                                                          \
            {                                                                  \
                    FIX16(x),                                                  \
                    FIX16(y)                                                   \
            }                                                                  \
    }

typedef union {
        struct {
                fix16_t x;
                fix16_t y;
        };

        fix16_t comp[2];
} __packed __aligned(4) fix16_vec2_t;

extern fix16_t fix16_vec2_dot(const fix16_vec2_t *, const fix16_vec2_t *);
extern fix16_t fix16_vec2_length(const fix16_vec2_t *);
extern void fix16_vec2_add(const fix16_vec2_t *, const fix16_vec2_t *, fix16_vec2_t *);
extern void fix16_vec2_dup(const fix16_vec2_t *, fix16_vec2_t *);
extern void fix16_vec2_normalize(fix16_vec2_t *);
extern void fix16_vec2_normalized(const fix16_vec2_t *, fix16_vec2_t *);
extern void fix16_vec2_scale(fix16_t, fix16_vec2_t *);
extern void fix16_vec2_scaled(fix16_t, const fix16_vec2_t *, fix16_vec2_t *);
extern void fix16_vec2_str(const fix16_vec2_t *, char *, int);
extern void fix16_vec2_sub(const fix16_vec2_t *, const fix16_vec2_t *, fix16_vec2_t *);
extern void fix16_vec2_zero(fix16_vec2_t *);

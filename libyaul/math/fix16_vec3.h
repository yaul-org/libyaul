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

union fix16_mat3;
typedef union fix16_mat3 fix16_mat3_t;

extern fix16_t fix16_vec3_angle(const fix16_vec3_t *, const fix16_vec3_t *);
extern fix16_t fix16_vec3_dot(const fix16_vec3_t *, const fix16_vec3_t *);
extern fix16_t fix16_vec3_length(const fix16_vec3_t *);
extern void fix16_vec3_add(const fix16_vec3_t *, const fix16_vec3_t *, fix16_vec3_t *);
extern void fix16_vec3_cross(const fix16_vec3_t *, const fix16_vec3_t *, fix16_vec3_t *);
extern void fix16_vec3_dup(const fix16_vec3_t *, fix16_vec3_t *);
extern void fix16_vec3_mat3_multiply(const fix16_mat3_t *, const fix16_vec3_t *, fix16_vec3_t *);
extern void fix16_vec3_normalize(fix16_vec3_t *);
extern void fix16_vec3_normalized(const fix16_vec3_t *, fix16_vec3_t *);
extern void fix16_vec3_scale(fix16_t, fix16_vec3_t *);
extern void fix16_vec3_scaled(const fix16_t, const fix16_vec3_t *, fix16_vec3_t *);
extern void fix16_vec3_str(const fix16_vec3_t *, char *, int);
extern void fix16_vec3_sub(const fix16_vec3_t *, const fix16_vec3_t *, fix16_vec3_t *);
extern void fix16_vec3_zero(fix16_vec3_t *);

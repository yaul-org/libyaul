#ifndef __libfixmath_fix16_h__
#define __libfixmath_fix16_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* These options may let the optimizer to remove some calls to the functions.
 * Refer to http://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
 */
#ifndef FIXMATH_FUNC_ATTRS
#ifdef __GNUC__
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 6)
#define FIXMATH_FUNC_ATTRS __attribute__((leaf, nothrow, const))
#else
#define FIXMATH_FUNC_ATTRS __attribute__((nothrow, const))
#endif
#else
#define FIXMATH_FUNC_ATTRS
#endif /* __GNUC__ */
#endif /* !FIXMATH_FUNC_ATTRS */

#include <sys/cdefs.h>

#include <stdint.h>

typedef int32_t fix16_t;

static const fix16_t FOUR_DIV_PI  = 0x145F3;            /*!< Fix16 value of 4/PI */
static const fix16_t _FOUR_DIV_PI2 = 0xFFFF9840;        /*!< Fix16 value of -4/PI² */
static const fix16_t X4_CORRECTION_COMPONENT = 0x399A;  /*!< Fix16 value of 0.225 */
static const fix16_t PI_DIV_4 = 0x0000C90F;             /*!< Fix16 value of PI/4 */
static const fix16_t THREE_PI_DIV_4 = 0x00025B2F;       /*!< Fix16 value of 3PI/4 */

static const fix16_t fix16_maximum  = 0x7FFFFFFF; /*!< the maximum value of fix16_t */
static const fix16_t fix16_minimum  = 0x80000000; /*!< the minimum value of fix16_t */
static const fix16_t fix16_overflow = 0x80000000; /*!< the value used to indicate overflows when FIXMATH_NO_OVERFLOW is not specified */

static const fix16_t fix16_pi  = 205887;     /*!< fix16_t value of pi */
static const fix16_t fix16_e   = 178145;     /*!< fix16_t value of e */
static const fix16_t fix16_one = 0x00010000; /*!< fix16_t value of 1 */

/* Conversion functions between fix16_t and float/integer.
 * These are inlined to allow compiler to optimize away constant numbers
 */
static inline fix16_t fix16_from_int(int a) {
        return a * fix16_one;
}

static inline float fix16_to_float(fix16_t a) {
        return (float)a / fix16_one;
}

static inline double fix16_to_double(fix16_t a) {
        return (double)a / fix16_one;
}

static inline int fix16_to_int(fix16_t a) {
#ifdef FIXMATH_NO_ROUNDING
        return (a >> 16);
#else
        if (a >= 0) {
                return (a + (fix16_one >> 1)) / fix16_one;
        }
        return (a - (fix16_one >> 1)) / fix16_one;
#endif
}

static inline int fix16_to_round_int(fix16_t a) {
        return (a >= 0)
            ? ((a + (fix16_one >> 1)) / fix16_one)
            : ((a - (fix16_one >> 1)) / fix16_one);
}

static inline fix16_t fix16_from_float(float a) {
        float temp = a * fix16_one;
#ifndef FIXMATH_NO_ROUNDING
        temp += (temp >= 0) ? 0.5f : -0.5f;
#endif /* !FIXMATH_NO_ROUNDING */
        return (fix16_t)temp;
}

static inline fix16_t fix16_from_double(double a) {
        double temp = a * fix16_one;
#ifndef FIXMATH_NO_ROUNDING
        temp += (temp >= 0) ? 0.5f : -0.5f;
#endif /* !FIXMATH_NO_ROUNDING */
        return (fix16_t)temp;
}

static inline fix16_t fix16_integral(fix16_t a) {
        return (a & 0xFFFF0000);
}

static inline fix16_t fix16_fractional(fix16_t a) {
        return (a & 0x0000FFFF);
}

/* Macro for defining fix16_t constant values.
   The functions above can't be used from e.g. global variable initializers,
   and their names are quite long also. This macro is useful for constants
   springled alongside code, e.g. F16(1.234).

   Note that the argument is evaluated multiple times, and also otherwise
   you should only use this for constant values. For runtime-conversions,
   use the functions above.
*/
#define F16(x) ((fix16_t)(((x) >= 0) ? ((x) * 65536.0 + 0.5) : ((x) * 65536.0 - 0.5)))

static inline fix16_t fix16_abs(fix16_t x) {
        return (x < 0 ? -x : x);
}

static inline fix16_t fix16_floor(fix16_t x) {
        return (x & 0xFFFF0000UL);
}

static inline fix16_t fix16_ceil(fix16_t x) {
        return (x & 0xFFFF0000UL) + (x & 0x0000FFFFUL ? fix16_one : 0);
}

static inline fix16_t fix16_min(fix16_t x, fix16_t y) {
        return (x < y ? x : y);
}

static inline fix16_t fix16_max(fix16_t x, fix16_t y) {
        return (x > y ? x : y);
}

static inline fix16_t fix16_clamp(fix16_t x, fix16_t lo, fix16_t hi) {
        return fix16_min(fix16_max(x, lo), hi);
}

/* Subtraction and addition with (optional) overflow detection. */
static inline fix16_t fix16_add(fix16_t a0, fix16_t a1) {
        return (a0 + a1);
}

static inline fix16_t fix16_sub(fix16_t a0, fix16_t a1) {
        return (a0 - a1);
}

static inline fix16_t fix16_mul(fix16_t a0, fix16_t a1) {
        return ((int64_t)a0 * a1) >> 16;
}

extern fix16_t fix16_overflow_add(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_overflow_sub(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

/* Saturatingrithmetic */
extern fix16_t fix16_overflow_sadd(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_overflow_ssub(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

/*! Multiplies the two given fix16_t's and returns the result.
*/
extern fix16_t fix16_overflow_mul(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

/*! Divides the first given fix16_t by the second and returns the result.
*/
extern fix16_t fix16_div(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

#ifndef FIXMATH_NO_OVERFLOW
/*! Performs a saturated multiplication (overflow-protected) of the two given fix16_t's and returns the result.
*/
extern fix16_t fix16_smul(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

/*! Performs a saturated division (overflow-protected) of the first fix16_t by the second and returns the result.
*/
extern fix16_t fix16_sdiv(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;
#endif /* !FIXMATH_NO_OVERFLOW */

/*! Divides the first given fix16_t by the second and returns the result.
*/
extern fix16_t fix16_mod(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;



/*! Returns the linear interpolation: (inArg0 * (1 - inFract)) + (inArg1 * inFract)
*/

extern fix16_t fix16_lerp(fix16_t, fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

extern fix16_t fix16_lerp8(fix16_t, fix16_t, uint8_t) FIXMATH_FUNC_ATTRS;
extern fix16_t fix16_lerp16(fix16_t, fix16_t, uint16_t) FIXMATH_FUNC_ATTRS;
#ifndef FIXMATH_NO_64BIT
extern fix16_t fix16_lerp32(fix16_t, fix16_t, uint32_t) FIXMATH_FUNC_ATTRS;
#endif /* !FIXMATH_NO_OVERFLOW */



/*! Returns the sine of the given fix16_t.
*/
extern fix16_t fix16_sin_parabola(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the sine of the given fix16_t.
*/
extern fix16_t fix16_sin(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the cosine of the given fix16_t.
*/
extern fix16_t fix16_cos(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the tangent of the given fix16_t.
*/
extern fix16_t fix16_tan(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the arcsine of the given fix16_t.
*/
extern fix16_t fix16_asin(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the arccosine of the given fix16_t.
*/
extern fix16_t fix16_acos(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the arctangent of the given fix16_t.
*/
extern fix16_t fix16_atan(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the arctangent of inY/inX.
*/
extern fix16_t fix16_atan2(fix16_t, fix16_t) FIXMATH_FUNC_ATTRS;

static const fix16_t fix16_rad_to_deg_mult = 3754936;

static inline fix16_t fix16_rad_to_deg(fix16_t radians) {
        return fix16_mul(radians, fix16_rad_to_deg_mult);
}

static const fix16_t fix16_deg_to_rad_mult = 1144;

static inline fix16_t fix16_deg_to_rad(fix16_t degrees) {
        return fix16_mul(degrees, fix16_deg_to_rad_mult);
}

/*! Returns the square root of the given fix16_t.
*/
extern fix16_t fix16_sqrt(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the square of the given fix16_t.
*/
static inline fix16_t fix16_sq(fix16_t x) {
        return fix16_mul(x, x);
}

/*! Returns the exponent (e^) of the given fix16_t.
*/
extern fix16_t fix16_exp(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the natural logarithm of the given fix16_t.
 */
extern fix16_t fix16_log(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the base 2 logarithm of the given fix16_t.
 */
extern fix16_t fix16_log2(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Returns the saturated base 2 logarithm of the given fix16_t.
 */
extern fix16_t fix16_slog2(fix16_t) FIXMATH_FUNC_ATTRS;

/*! Convert fix16_t value to a string.
 * Required buffer length for largest values is 13 bytes.
 */
extern void fix16_to_str(fix16_t, char *, int);

/*! Convert string to a fix16_t value
 * Ignores spaces at beginning and end. Returns fix16_overflow if
 * value is too large or there were garbage characters.
 */
extern fix16_t fix16_from_str(const char *);

typedef union {
        struct {
                fix16_t x;
                fix16_t y;
        };

        fix16_t comp[2];
} __packed __aligned(4) fix16_vector2_t;

typedef union {
        struct {
                fix16_t x;
                fix16_t y;
                fix16_t z;
        };

        fix16_t comp[3];
} __packed __aligned(16) fix16_vector3_t;

typedef union {
        struct {
                fix16_t x;
                fix16_t y;
                fix16_t z;
                fix16_t w;
        };

        fix16_t comp[4];
} __packed __aligned(16) fix16_vector4_t;

/* Row-major matrix */
typedef union {
        struct {
                fix16_t m00, m01, m02; /* Row 0 */
                fix16_t m10, m11, m12; /* Row 1 */
                fix16_t m20, m21, m22; /* Row 2 */
        } comp;

        fix16_t arr[9];
        fix16_t frow[3][3];
        fix16_vector3_t row[3];
} __aligned(64) fix16_matrix3_t;

/* Row-major matrix */
typedef union {
        struct {
                fix16_t m00, m01, m02, m03; /* Row 0 */
                fix16_t m10, m11, m12, m13; /* Row 1 */
                fix16_t m20, m21, m22, m23; /* Row 2 */
                fix16_t m30, m31, m32, m33; /* Row 3 */
        } comp;

        fix16_t arr[16];
        fix16_t frow[4][4];
        fix16_vector4_t row[4];
} __aligned(64) fix16_matrix4_t;

#define FIX16_VECTOR2_INITIALIZER(x, y)                                        \
    {                                                                          \
            {                                                                  \
                    F16(x),                                                    \
                    F16(y)                                                     \
            }                                                                  \
    }

extern fix16_t fix16_vector2_dot(const fix16_vector2_t *,
    const fix16_vector2_t *);
extern fix16_t fix16_vector2_length(const fix16_vector2_t *);
extern void fix16_vector2_add(const fix16_vector2_t *, const fix16_vector2_t *,
    fix16_vector2_t *);
extern void fix16_vector2_dup(const fix16_vector2_t *, fix16_vector2_t *);
extern void fix16_vector2_normalize(fix16_vector2_t *);
extern void fix16_vector2_normalized(const fix16_vector2_t *,
    fix16_vector2_t *);
extern void fix16_vector2_scale(fix16_t, fix16_vector2_t *);
extern void fix16_vector2_scaled(fix16_t, const fix16_vector2_t *,
    fix16_vector2_t *);
extern void fix16_vector2_str(const fix16_vector2_t *, char *, int);
extern void fix16_vector2_sub(const fix16_vector2_t *, const fix16_vector2_t *,
    fix16_vector2_t *);
extern void fix16_vector2_zero(fix16_vector2_t *);

#define FIX16_VECTOR3_INITIALIZER(x, y, z)                                     \
    {                                                                          \
            {                                                                  \
                    F16(x),                                                    \
                    F16(y),                                                    \
                    F16(z)                                                     \
            }                                                                  \
    }

extern fix16_t fix16_vector3_angle(const fix16_vector3_t *,
    const fix16_vector3_t *);
extern fix16_t fix16_vector3_dot(const fix16_vector3_t *,
    const fix16_vector3_t *);
extern fix16_t fix16_vector3_length(const fix16_vector3_t *);
extern void fix16_vector3_add(const fix16_vector3_t *, const fix16_vector3_t *,
    fix16_vector3_t *);
extern void fix16_vector3_cross(const fix16_vector3_t *,
    const fix16_vector3_t *, fix16_vector3_t *);
extern void fix16_vector3_dup(const fix16_vector3_t *, fix16_vector3_t *);
extern void fix16_vector3_matrix3_multiply(const fix16_matrix3_t *,
    const fix16_vector3_t *, fix16_vector3_t *);
extern void fix16_vector3_normalize(fix16_vector3_t *);
extern void fix16_vector3_normalized(const fix16_vector3_t *,
    fix16_vector3_t *);
extern void fix16_vector3_scale(fix16_t, fix16_vector3_t *);
extern void fix16_vector3_scaled(const fix16_t, const fix16_vector3_t *,
    fix16_vector3_t *);
extern void fix16_vector3_str(const fix16_vector3_t *, char *, int);
extern void fix16_vector3_sub(const fix16_vector3_t *, const fix16_vector3_t *,
    fix16_vector3_t *);
extern void fix16_vector3_zero(fix16_vector3_t *);

extern void fix16_matrix3_dup(const fix16_matrix3_t *, fix16_matrix3_t *);
extern void fix16_matrix3_identity(fix16_matrix3_t *);
extern void fix16_matrix3_inverse(const fix16_matrix3_t *, fix16_matrix3_t *);
extern void fix16_matrix3_multiply(const fix16_matrix3_t *,
    const fix16_matrix3_t *, fix16_matrix3_t *);
extern void fix16_matrix3_str(const fix16_matrix3_t *, char *, int);
extern void fix16_matrix3_transpose(const fix16_matrix3_t *, fix16_matrix3_t *);
extern void fix16_matrix3_zero(fix16_matrix3_t *);

#define FIX16_VECTOR4_INITIALIZER(x, y, z, w)                                  \
    {                                                                          \
            F16(x),                                                            \
            F16(y),                                                            \
            F16(z),                                                            \
            F16(w),                                                            \
    }

#ifdef FIXMATH_NO_OVERFLOW
static inline fix16_t fix16_vector4_dot(const fix16_vector4_t *v0,
    const fix16_vector4_t *v1) {
        return fix16_add(fix16_mul(v0->x, v1->x),
            fix16_add(fix16_mul(v0->y, v1->y), fix16_mul(v0->z, v1->z)));
}
#endif /* !FIXMATH_NO_OVERFLOW */

extern fix16_t fix16_vector4_length(const fix16_vector4_t *);
extern void fix16_vector4_add(const fix16_vector4_t *, const fix16_vector4_t *,
    fix16_vector4_t *);
extern void fix16_vector4_cross(const fix16_vector4_t *,
    const fix16_vector4_t *, fix16_vector4_t *);
extern void fix16_vector4_dup(const fix16_vector4_t *, fix16_vector4_t *);
extern void fix16_vector4_matrix4_multiply(const fix16_matrix4_t *,
    const fix16_vector4_t *, fix16_vector4_t *);
extern void fix16_vector4_matrix4_multiply2(const fix16_matrix4_t *,
    const fix16_vector4_t *, fix16_vector4_t *);
extern void fix16_vector4_normalize(fix16_vector4_t *);
extern void fix16_vector4_normalized(const fix16_vector4_t *,
    fix16_vector4_t *);
extern void fix16_vector4_scale(fix16_t, fix16_vector4_t *);
extern void fix16_vector4_scaled(fix16_t, const fix16_vector4_t *,
    fix16_vector4_t *);
extern void fix16_vector4_str(const fix16_vector4_t *, char *, int);
extern void fix16_vector4_sub(const fix16_vector4_t *, const fix16_vector4_t *,
    fix16_vector4_t *);
extern void fix16_vector4_zero(fix16_vector4_t *);

extern void fix16_matrix4_dup(const fix16_matrix4_t *, fix16_matrix4_t *);
extern void fix16_matrix4_identity(fix16_matrix4_t *);
extern void fix16_matrix4_inverse(const fix16_matrix4_t *, fix16_matrix4_t *);
extern void fix16_matrix4_multiply(const fix16_matrix4_t *,
    const fix16_matrix4_t *, fix16_matrix4_t *);
extern void fix16_matrix4_multiply2(const fix16_matrix4_t *,
    const fix16_matrix4_t *, fix16_matrix4_t *);
extern void fix16_matrix4_str(const fix16_matrix4_t *, char *, int);
extern void fix16_matrix4_transpose(const fix16_matrix4_t *, fix16_matrix4_t *);
extern void fix16_matrix4_zero(fix16_matrix4_t *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !__libfixmath_fix16_h__ */

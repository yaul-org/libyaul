/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes <abra185@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_FIX16_VEC3_H_
#define _YAUL_GAMEMATH_FIX16_VEC3_H_

#include <cpu/divu.h>

#include <gamemath/fix16.h>

/// @addtogroup MATH_FIX16_VECTOR
/// @defgroup MATH_FIX16_VEC3 3-vector
/// @ingroup MATH_FIX16_VECTOR
/// @{

#if !defined(__cplusplus)
/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
/// @param z Not yet documented.
#define FIX16_VEC3_INITIALIZER(x, y, z)                                        \
{                                                                              \
    FIX16(x),                                                                  \
    FIX16(y),                                                                  \
    FIX16(z)                                                                   \
}                                                                              \

/// @brief Not yet documented.
///
/// @param x Not yet documented.
/// @param y Not yet documented.
/// @param z Not yet documented.
#define FIX16_VEC3(x, y, z)                                                    \
((fix16_vec3_t){                                                               \
    FIX16(x),                                                                  \
    FIX16(y),                                                                  \
    FIX16(z)                                                                   \
})
#endif /* !__cplusplus */

#if !defined(__cplusplus)
/// @brief Not yet documented.
typedef struct fix16_vec3 {
    /// @brief Not yet documented.
    fix16_t x;
    /// @brief Not yet documented.
    fix16_t y;
    /// @brief Not yet documented.
    fix16_t z;
} fix16_vec3_t;
#else
/// @brief Not yet documented.
struct fix16_vec3_t {
    /// @brief Not yet documented.
    fix16_t x;
    /// @brief Not yet documented.
    fix16_t y;
    /// @brief Not yet documented.
    fix16_t z;

    fix16_vec3_t() { }
    fix16_vec3_t(fix16_vec3_t&&)      = default;
    fix16_vec3_t(const fix16_vec3_t&) = default;

    constexpr inline fix16_vec3_t(fix16_t x_, fix16_t y_, fix16_t z_);

    ~fix16_vec3_t() = default;

    fix16_vec3_t& operator=(const fix16_vec3_t& other) = default;
    fix16_vec3_t& operator=(fix16_vec3_t&& other)      = default;

    inline const fix16_vec3_t operator+(const fix16_vec3_t& other) const;
    inline const fix16_vec3_t operator-(const fix16_vec3_t& other) const;
    inline const fix16_vec3_t operator-() const;
    inline const fix16_vec3_t operator*(const fix16_t& other) const;
    inline const fix16_vec3_t operator*(int32_t other) const;
    inline const fix16_vec3_t operator*(uint32_t other) const;
    inline const fix16_vec3_t operator/(const fix16_t& other) const;

    inline fix16_vec3_t& operator+=(const fix16_vec3_t& rhs);
    inline fix16_vec3_t& operator-=(const fix16_vec3_t& rhs);
    inline fix16_vec3_t& operator*=(fix16_t rhs);
    inline fix16_vec3_t& operator*=(int32_t rhs);
    inline fix16_vec3_t& operator/=(fix16_t rhs);

    bool is_near_zero(const fix16_t epsilon = 0.001_fp) const;

    inline fix16_t length() const;

    inline fix16_t length_sqrt() const;

    inline void normalize();

    inline fix16_vec3_t normalized();

    inline void start_normalization() const;

    void end_normalization();

    static constexpr inline fix16_vec3_t zero();

    static constexpr inline fix16_vec3_t unit_x();

    static constexpr inline fix16_vec3_t unit_y();

    static constexpr inline fix16_vec3_t unit_z();

    static inline fix16_t dot_product(const fix16_vec3_t& a, const fix16_vec3_t& b);

    static inline fix16_vec3_t cross_product(const fix16_vec3_t& a, const fix16_vec3_t& b);

    static fix16_vec3_t reflect(const fix16_vec3_t& v, const fix16_vec3_t& normal);

    inline void to_string(char* buffer, int32_t decimals = 7) const;

    static inline constexpr fix16_vec3_t from_double(double x, double y, double z);
};
#endif /* !__cplusplus */

#if !defined(__cplusplus)
/// @brief Not yet documented.
///
/// @param result Not yet documented.
static inline void __always_inline
fix16_vec3_zero(fix16_vec3_t *result)
{
    result->x = FIX16(0.0);
    result->y = FIX16(0.0);
    result->z = FIX16(0.0);
}

/// @brief Not yet documented.
///
/// @param[out] result Not yet documented.
/// @param      x      Not yet documented.
/// @param      y      Not yet documented.
/// @param      z      Not yet documented.
static inline void __always_inline
fix16_vec3_set(fix16_vec3_t *result, fix16_t x, fix16_t y, fix16_t z)
{
    result->x = x;
    result->y = y;
    result->z = z;
}

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec3_dup(const fix16_vec3_t * __restrict v0,
  fix16_vec3_t * __restrict result)
{
    result->x = v0->x;
    result->y = v0->y;
    result->z = v0->z;
}

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param      v1     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec3_add(const fix16_vec3_t * __restrict v0,
  const fix16_vec3_t * __restrict v1, fix16_vec3_t * __restrict result)
{
    result->x = v0->x + v1->x;
    result->y = v0->y + v1->y;
    result->z = v0->z + v1->z;
}

/// @brief Not yet documented.
///
/// @param      v1     Not yet documented.
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec3_sub(const fix16_vec3_t * __restrict v1,
  const fix16_vec3_t * __restrict const v0, fix16_vec3_t * __restrict const result)
{
    result->x = v1->x - v0->x;
    result->y = v1->y - v0->y;
    result->z = v1->z - v0->z;
}

/// @brief Not yet documented.
///
/// @param      scalar Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec3_scale(const fix16_t scalar, fix16_vec3_t *result)
{
    result->x = fix16_mul(scalar, result->x);
    result->y = fix16_mul(scalar, result->y);
    result->z = fix16_mul(scalar, result->z);
}

/// @brief Not yet documented.
///
/// @param      scalar Not yet documented.
/// @param      v      Not yet documented.
/// @param[out] result Not yet documented.
static inline void __always_inline
fix16_vec3_scaled(const fix16_t scalar, const fix16_vec3_t *v,
  fix16_vec3_t *result)
{
    result->x = fix16_mul(scalar, v->x);
    result->y = fix16_mul(scalar, v->y);
    result->z = fix16_mul(scalar, v->z);
}

/// @brief Not yet documented.
///
/// @param a Not yet documented.
/// @param b Not yet documented.
///
/// @returns The value.
__BEGIN_ASM
static inline fix16_t __always_inline
fix16_vec3_inline_dot(const fix16_vec3_t *a, const fix16_vec3_t *b)
{
    __register uint32_t aux0;
    __register uint32_t aux1;

    __declare_asm("\tclrmac\n"
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
__END_ASM
#endif /* !__cplusplus */

__BEGIN_DECLS

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
///
/// @returns The value.
extern fix16_t fix16_vec3_length(const fix16_vec3_t *v0);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
///
/// @returns The value.
extern fix16_t fix16_vec3_sqr_length(const fix16_vec3_t *v0);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
extern void fix16_vec3_normalize(fix16_vec3_t *v0);

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_vec3_normalized(const fix16_vec3_t * __restrict v0,
  fix16_vec3_t * __restrict result);

/// @brief Not yet documented.
///
/// @param v0 Not yet documented.
/// @param v1 Not yet documented.
///
/// @returns The value.
extern fix16_t fix16_vec3_dot(const fix16_vec3_t *v0, const fix16_vec3_t *v1);

/// @brief Not yet documented.
///
/// @param      v0     Not yet documented.
/// @param      v1     Not yet documented.
/// @param[out] result Not yet documented.
extern void fix16_vec3_cross(const fix16_vec3_t * const __restrict v0,
  const fix16_vec3_t * const __restrict v1, fix16_vec3_t * __restrict result);

/// @brief Not yet documented.
///
/// @param      v0 Not yet documented.
/// @param      v1 Not yet documented.
///
/// @returns The value.
extern fix16_t fix16_vec3_cross_mag(const fix16_vec3_t * __restrict v0,
  const fix16_vec3_t * __restrict v1);

/// @brief Not yet documented.
///
/// @param      v0       Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documunted.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_vec3_str(const fix16_vec3_t *v0, char *buffer, int32_t decimals);

__END_DECLS

#if defined(__cplusplus)
constexpr inline fix16_vec3_t::fix16_vec3_t(fix16_t x_, fix16_t y_, fix16_t z_) : x(x_), y(y_), z(z_) {}

inline const fix16_vec3_t fix16_vec3_t::operator+(const fix16_vec3_t& v) const { return fix16_vec3_t{x + v.x, y + v.y, z + v.z}; }
inline const fix16_vec3_t fix16_vec3_t::operator-(const fix16_vec3_t& v) const { return fix16_vec3_t{x - v.x, y - v.y, z - v.z}; }
inline const fix16_vec3_t fix16_vec3_t::operator-() const { return fix16_vec3_t{-x, -y, -z}; }
inline const fix16_vec3_t fix16_vec3_t::operator*(const fix16_t& value) const { return fix16_vec3_t{x * value, y * value, z * value}; }
inline const fix16_vec3_t fix16_vec3_t::operator*(const int32_t value) const { return fix16_vec3_t{x * value, y * value, z * value}; }
inline const fix16_vec3_t fix16_vec3_t::operator*(const uint32_t value) const { return fix16_vec3_t{x * value, y * value, z * value}; }

inline fix16_vec3_t& fix16_vec3_t::operator+=(const fix16_vec3_t& v) {
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
}

inline fix16_vec3_t& fix16_vec3_t::operator-=(const fix16_vec3_t& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
}

inline fix16_vec3_t& fix16_vec3_t::operator*=(const fix16_t value) {
    x = x * value;
    y = y * value;
    z = z * value;

    return *this;
}

inline fix16_vec3_t& fix16_vec3_t::operator/=(const fix16_t value) {
    const fix16_t inv_value = 1.0_fp / value;
    x = x * inv_value;
    y = y * inv_value;
    z = z * inv_value;

    return *this;
}

inline fix16_t fix16_vec3_t::length() const { return fix16_vec3_length(this); }

inline fix16_t fix16_vec3_t::length_sqrt() const { return fix16_vec3_sqr_length(this); }

inline void fix16_vec3_t::normalize() { fix16_vec3_normalize(this); }

inline fix16_vec3_t fix16_vec3_t::normalized() {
    fix16_vec3_t result;
    fix16_vec3_normalized(this, &result);

    return result;
}

inline void fix16_vec3_t::start_normalization() const { cpu_divu_fix16_set(1.0_fp, length()); }

constexpr inline fix16_vec3_t fix16_vec3_t::zero() { return fix16_vec3_t{0.0_fp, 0.0_fp, 0.0_fp}; }

constexpr inline fix16_vec3_t fix16_vec3_t::unit_x() { return fix16_vec3_t{1.0_fp, 0.0_fp, 0.0_fp}; }

constexpr inline fix16_vec3_t fix16_vec3_t::unit_y() { return fix16_vec3_t{0.0_fp, 1.0_fp, 0.0_fp}; }

constexpr inline fix16_vec3_t fix16_vec3_t::unit_z() { return fix16_vec3_t{0.0_fp, 0.0_fp, 1.0_fp}; }

constexpr inline fix16_vec3_t fix16_vec3_t::from_double(double x, double y, double z) {
    return fix16_vec3_t{fix16_t::from_double(x), fix16_t::from_double(y), fix16_t::from_double(z)};
}

inline fix16_t fix16_vec3_t::dot_product(const fix16_vec3_t& a, const fix16_vec3_t& b) {
    return fix16_vec3_dot(&a, &b);
}

inline fix16_vec3_t fix16_vec3_t::cross_product(const fix16_vec3_t& a, const fix16_vec3_t& b) {
    fix16_vec3_t result;
    fix16_vec3_cross(&a, &b, &result);

    return result;
}

inline void fix16_vec3_t::to_string(char* buffer, int32_t decimals) const { fix16_vec3_str(this, buffer, decimals); }
#endif /* __cplusplus */

/// @}

#endif /* !_YAUL_GAMEMATH_FIX16_VEC3_H_ */

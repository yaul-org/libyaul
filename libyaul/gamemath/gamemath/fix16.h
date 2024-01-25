/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

/*-
 * Copyright (c) Authors of libfixmath
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _YAUL_GAMEMATH_FIX16_H_
#define _YAUL_GAMEMATH_FIX16_H_

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdint.h>

#include <gamemath/defs.h>

/// @defgroup MATH_FIX16 Fix16
/// @ingroup MATH

/// @defgroup MATH_FIX16_VECTOR Fix16 Vector
/// @ingroup MATH

/// @defgroup MATH_FIX16_MATRIX Fix16 Matrix
/// @ingroup MATH

/// @defgroup MATH_FIX16_PLANES Fix16 Planes
/// @ingroup MATH

/// @defgroup MATH_FIX16_TRIG Fix16 Trigonometry
/// @ingroup MATH

/// @addtogroup MATH_FIX16
/// @{

#if !defined(__cplusplus)
/// @brief Macro for defininge @p fix16_t constant values.
///
/// @note The argument is evaluated multiple times, and also otherwise you
/// should only use this for constant values. For runtime-conversions, use the
/// inlined functions below.
///
/// @param x The constant value.
#define FIX16(x) ((fix16_t)(((x) >= 0)                                         \
    ? ((double)(x) * 65536.0 + 0.5)                                            \
    : ((double)(x) * 65536.0 - 0.5)))
#endif /* !__cplusplus */

#if !defined(__cplusplus)
/// @brief Fixed point Q16.16.
typedef int32_t fix16_t;
#else
/// @brief Fixed point Q16.16.
struct fix16_t {
    int32_t value;

    fix16_t() = default;

    explicit constexpr fix16_t(int32_t v);

    explicit constexpr fix16_t(uint32_t v);

    // For dealing with C
    explicit constexpr operator int32_t() const { return value; }
    explicit constexpr operator uint32_t() const { return value; }

    inline fix16_t operator+(fix16_t other) const;
    inline fix16_t operator-(fix16_t other) const;
    constexpr inline fix16_t operator-() const;
    inline fix16_t operator*(fix16_t other) const;
    inline fix16_t operator*(int32_t other) const;
    inline fix16_t operator*(uint32_t other) const;
    inline fix16_t operator/(fix16_t other) const;
    inline fix16_t operator>>(int32_t i) const;
    inline fix16_t operator<<(int32_t i) const;

    inline fix16_t& operator+=(fix16_t rhs);
    inline fix16_t& operator-=(fix16_t rhs);
    inline fix16_t& operator*=(fix16_t rhs);
    inline fix16_t& operator*=(int32_t rhs);
    inline fix16_t& operator*=(uint32_t rhs);
    inline fix16_t& operator/=(fix16_t rhs);
    inline fix16_t& operator>>=(int32_t i);
    inline fix16_t& operator<<=(int32_t i);

    inline bool operator<(fix16_t other) const;
    inline bool operator<(int32_t other) const;
    inline bool operator>(fix16_t other) const;
    inline bool operator>(int32_t other) const;
    inline bool operator<=(fix16_t other) const;
    inline bool operator<=(int32_t other) const;
    inline bool operator>=(fix16_t other) const;
    inline bool operator>=(int32_t other) const;
    inline bool operator==(fix16_t other) const;
    inline bool operator==(int32_t other) const;

    constexpr inline int16_t fractional() const;

    inline bool is_near_zero(fix16_t epsilon = from_double(0.001)) const;

    inline bool is_near(fix16_t other, fix16_t epsilon = from_double(0.001)) const;

    inline bool is_negative() const;

    inline bool is_positive() const;

    constexpr inline int16_t to_int() const;

    inline size_t to_string(char* buffer, int32_t decimals = 7) const;

    constexpr static inline fix16_t from_double(double value);
};

static_assert(sizeof(fix16_t) == 4);
#endif /* !__cplusplus */

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
///
/// @returns The value.
__BEGIN_ASM
static inline int32_t __always_inline
fix16_low_mul(fix16_t a, fix16_t b)
{
    __register int32_t out;

    __declare_asm("\tdmuls.l %[a], %[b]\n"
                  "\tsts macl, %[out]\n"
                  /* Output */
                  : [out] "=&r" (out)
                  /* Input */
                  : [a] "r" (a),
                    [b] "r" (b)
                  : "macl");

    return out;
}
__END_ASM

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
///
/// @returns The value.
__BEGIN_ASM
static inline int32_t __always_inline
fix16_high_mul(fix16_t a, fix16_t b)
{
    __register int16_t out;

    __declare_asm("\tdmuls.l %[a], %[b]\n"
                  "\tsts mach, %[out]\n"
                  /* Output */
                  : [out] "=&r" (out)
                  /* Input */
                  : [a] "r" (a),
                    [b] "r" (b)
                  : "mach");

    return out;
}
__END_ASM

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
///
/// @returns The value.
__BEGIN_ASM
static inline fix16_t __always_inline
fix16_mul(fix16_t a, fix16_t b)
{
    __register uint32_t mach;
    __register fix16_t out;

    __declare_asm("\tdmuls.l %[a], %[b]\n"
                  "\tsts mach, %[mach]\n"
                  "\tsts macl, %[out]\n"
                  "\txtrct %[mach], %[out]\n"
                  /* Output */
                  : [mach] "=&r" (mach),
                    [out] "=&r" (out)
                  /* Input */
                  : [a] "r" (a),
                    [b] "r" (b)
                  : "mach", "macl");

    return out;
}
__END_ASM

#if !defined(__cplusplus)
/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_int32_from(int32_t value)
{
    return (value * FIX16(1.0));
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline int32_t __always_inline
fix16_int32_to(fix16_t value)
{
    return (value >> 16);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline int32_t __always_inline
fix16_round_int32_to(fix16_t value)
{
    if (value >= 0) {
        return ((value + (FIX16(1.0) / 2)) / FIX16(1.0));
    }

    return ((value - (FIX16(1.0) / 2)) / FIX16(1.0));
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline uint32_t __always_inline
fix16_integral(fix16_t value)
{
    return (value & 0xFFFF0000UL);
}

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline uint32_t __always_inline
fix16_fractional(fix16_t value)
{
    return (value & 0x0000FFFFUL);
}

// TODO: Does not compile for C++
/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_floor(fix16_t value)
{
    return (value & 0xFFFF0000);
}

// TODO: Does not compile for C++
/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_ceil(fix16_t value)
{
    return ((value & 0xFFFF0000UL) + ((value & 0x0000FFFFUL) ? FIX16(1.0) : 0));
}

/// @brief Not yet documented.
///
/// @param x Operand.
/// @param y Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_min(const fix16_t x, const fix16_t y)
{
        return ((x < y) ? x : y);
}

/// @brief Not yet documented.
///
/// @param x Operand.
/// @param y Operand.
///
/// @returns The value.
static inline fix16_t __always_inline
fix16_max(const fix16_t x, const fix16_t y)
{
        return ((x > y) ? x : y);
}
#endif /* !__cplusplus */

__BEGIN_DECLS

/// @brief Not yet documented.
///
/// @param a        Operand.
/// @param b        Operand.
/// @param sum[out] Sum.
///
/// @returns If overflow occurred.
extern bool fix16_overflow_add(fix16_t a, fix16_t b, fix16_t *sum);

/// @brief Not yet documented.
///
/// @param a         Operand.
/// @param b         Operand.
/// @param diff[out] Sum.
///
/// @returns If overflow occurred.
extern bool fix16_overflow_sub(fix16_t a, fix16_t b, fix16_t *diff);

/// @brief Not yet documented.
///
/// @param dividend Operand.
/// @param divisor  Operand.
///
/// @returns The value.
extern fix16_t fix16_div(fix16_t dividend, fix16_t divisor);

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
/// @param t Operand.
///
/// @returns The value.
extern fix16_t fix16_lerp(fix16_t a, fix16_t b, fix16_t t);

/// @brief Not yet documented.
///
/// @param a Operand.
/// @param b Operand.
/// @param t Operand.
///
/// @returns The value.
extern fix16_t fix16_lerp8(fix16_t a, fix16_t b, const uint8_t t);

/// @brief Not yet documented.
///
/// @param value Operand.
///
/// @returns The value.
extern fix16_t fix16_sqrt(fix16_t value);

/// @brief Not yet documented.
///
/// @param      value    Not yet documented.
/// @param[out] buffer   Not yet documented.
/// @param      decimals Not yet documented.
///
/// @returns The string length, not counting the `NUL` character.
extern size_t fix16_str(fix16_t value, char *buffer, int32_t decimals);

__END_DECLS

#if defined(__cplusplus)
static inline fix16_t abs(fix16_t a) {
    return fix16_t{::abs(a.value)};
}

constexpr fix16_t operator"" _fp(long double v) { return fix16_t::from_double(v); }

constexpr fix16_t::fix16_t(int32_t v) : value(v) {}

constexpr fix16_t::fix16_t(uint32_t v) : value(v) {}

inline fix16_t fix16_t::operator+(fix16_t other) const { return fix16_t{value + other.value}; }
inline fix16_t fix16_t::operator-(fix16_t other) const { return fix16_t{value - other.value}; }
constexpr inline fix16_t fix16_t::operator-() const { return fix16_t{-value}; }
inline fix16_t fix16_t::operator*(fix16_t other) const { return fix16_mul(*this, other); }
inline fix16_t fix16_t::operator*(int32_t other) const { return fix16_mul(*this, fix16_t{other}); }
inline fix16_t fix16_t::operator*(uint32_t other) const { return fix16_mul(*this, fix16_t{other}); }
inline fix16_t fix16_t::operator/(fix16_t other) const { return fix16_div(*this, other); }

inline fix16_t fix16_t::operator>>(int32_t i) const { return fix16_t{value >> i}; }
inline fix16_t fix16_t::operator<<(int32_t i) const { return fix16_t{value << i}; }

inline fix16_t& fix16_t::operator+=(fix16_t rhs) {
    value = value + rhs.value;
    return *this;
}

inline fix16_t& fix16_t::operator-=(fix16_t rhs) {
    value = value - rhs.value;
    return *this;
}

inline fix16_t& fix16_t::operator*=(fix16_t rhs) {
    *this = fix16_mul(*this, rhs);
    return *this;
}

inline fix16_t& fix16_t::operator*=(int32_t rhs) {
    *this = fix16_mul(*this, fix16_t{rhs});
    return *this;
}

inline fix16_t& fix16_t::operator*=(uint32_t rhs) {
    *this = fix16_mul(*this, fix16_t{rhs});
    return *this;
}

inline fix16_t& fix16_t::operator/=(fix16_t rhs) {
    *this = fix16_div(*this, rhs);
    return *this;
}

inline fix16_t& fix16_t::operator>>=(int32_t i) {
    value = value >> i;
    return *this;
}

inline fix16_t& fix16_t::operator<<=(int32_t i) {
    value = value >> i;
    return *this;
}

inline bool fix16_t::operator<(fix16_t other) const { return value < other.value; }
inline bool fix16_t::operator<(int32_t other) const { return value < other; }
inline bool fix16_t::operator>(fix16_t other) const { return value > other.value; }
inline bool fix16_t::operator>(int32_t other) const { return value > other; }
inline bool fix16_t::operator<=(fix16_t other) const { return value <= other.value; }
inline bool fix16_t::operator<=(int32_t other) const { return value <= other; }
inline bool fix16_t::operator>=(fix16_t other) const { return value >= other.value; }
inline bool fix16_t::operator>=(int32_t other) const { return value >= other; }
inline bool fix16_t::operator==(fix16_t other) const { return value == other.value; }
inline bool fix16_t::operator==(int32_t other) const { return value == other; }

constexpr inline fix16_t fix16_t::from_double(double value) {
    return fix16_t{static_cast<int32_t>((value >= 0) ? ((value * 65536.0) + 0.5) : ((value * 65536.0) - 0.5))};
}

static inline fix16_t sqrt(fix16_t value) { return fix16_sqrt(value); }

constexpr inline int16_t fix16_t::fractional() const { return (value & 0x0000FFFF); }

inline bool fix16_t::is_near_zero(fix16_t epsilon) const { return abs(*this) <= epsilon; }

inline bool fix16_t::is_near(fix16_t other, fix16_t epsilon) const { return abs(*this - other) <= epsilon; }

inline bool fix16_t::is_negative() const { return (value < 0); }

inline bool fix16_t::is_positive() const { return (value >= 0); }

constexpr inline int16_t fix16_t::to_int() const { return (value / 65536); }

inline size_t fix16_t::to_string(char* buffer, int32_t decimals) const { return fix16_str(*this, buffer, decimals); }
#endif /* __cplusplus */

/// @}

#endif /* !_YAUL_GAMEMATH_FIX16_H_ */

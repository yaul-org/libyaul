/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_DEFS_H_
#define _YAUL_GAMEMATH_DEFS_H_

#include <stdint.h>

/* pi */
#define M_PI       (3.14159265358979323846f)
/* pi/2 */
#define M_PI_2     (1.57079632679489661923f)
/* pi/4 */
#define M_PI_4     (0.78539816339744830962f)
/* 1/pi */
#define M_1_PI     (0.31830988618379067154f)
/* 2/pi */
#define M_2_PI     (0.63661977236758134308f)
/* 2/sqrt(pi) */
#define M_2_SQRTPI (1.12837916709551257390f)
/* sqrt(2) */
#define M_SQRT2    (1.41421356237309504880f)
/* 1/sqrt(2) */
#define M_SQRT1_2  (0.70710678118654752440f)

/* Shamelessly stolen from:
 *   https://stackoverflow.com/a/73484677/604033 */

#ifndef min
#if !defined(__cplusplus)
#define __DEFINE_MIN(name, T)                                                  \
    static inline T name(T a, T b) { return (a < b) ? a : b; }

__DEFINE_MIN(min_bool, _Bool)
__DEFINE_MIN(min_char, char)
__DEFINE_MIN(min_schar, signed char)
__DEFINE_MIN(min_uchar, unsigned char)
__DEFINE_MIN(min_short, short)
__DEFINE_MIN(min_ushort, unsigned short)
__DEFINE_MIN(min_int, int)
__DEFINE_MIN(min_uint, unsigned)
__DEFINE_MIN(min_long, long)
__DEFINE_MIN(min_ulong, unsigned long)
__DEFINE_MIN(min_llong, long long)
__DEFINE_MIN(min_ullong, unsigned long long)

#undef __DEFINE_MIN

#define min(a, b)                                                              \
    _Generic(((a) < (b)) ? (a) : (b),                                          \
             _Bool: min_bool,                                                  \
              char: min_char,                                                  \
       signed char: min_schar,                                                 \
     unsigned char: min_uchar,                                                 \
             short: min_short,                                                 \
    unsigned short: min_ushort,                                                \
               int: min_int,                                                   \
          unsigned: min_uint,                                                  \
              long: min_long,                                                  \
     unsigned long: min_ulong,                                                 \
         long long: min_llong,                                                 \
unsigned long long: min_ullong,                                                \
         default: "bad type for min")(a, b)
#else
template <typename _T>
static inline const _T& min(const _T& _a, const _T& _b) {
  return (_a < _b) ? _a : _b;
}

template <typename _T, typename _Compare>
static inline const _T& min(const _T& _a, const _T& _b, _Compare _compare) {
  return _compare(_a, _b) ? _a : _b;
}
#endif
#endif /* !min */

#ifndef max
#if !defined(__cplusplus)
#define __DEFINE_MAX(name, T)                                                  \
    static inline T name(T a, T b) { return (a > b) ? a : b; }

__DEFINE_MAX(max_bool, _Bool)
__DEFINE_MAX(max_char, char)
__DEFINE_MAX(max_schar, signed char)
__DEFINE_MAX(max_uchar, unsigned char)
__DEFINE_MAX(max_short, short)
__DEFINE_MAX(max_ushort, unsigned short)
__DEFINE_MAX(max_int, int)
__DEFINE_MAX(max_uint, unsigned)
__DEFINE_MAX(max_long, long)
__DEFINE_MAX(max_ulong, unsigned long)
__DEFINE_MAX(max_llong, long long)
__DEFINE_MAX(max_ullong, unsigned long long)

#undef __DEFINE_MAX

#define max(a, b)                                                              \
    _Generic(((a) > (b)) ? (a) : (b),                                          \
             _Bool: max_bool,                                                  \
              char: max_char,                                                  \
       signed char: max_schar,                                                 \
     unsigned char: max_uchar,                                                 \
             short: max_short,                                                 \
    unsigned short: max_ushort,                                                \
               int: max_int,                                                   \
          unsigned: max_uint,                                                  \
              long: max_long,                                                  \
     unsigned long: max_ulong,                                                 \
         long long: max_llong,                                                 \
unsigned long long: max_ullong,                                                \
         default: "bad type for max")(a, b)
#else
template <typename _T>
static inline const _T& max(const _T& _a, const _T& _b) {
  return (_a > _b) ? _a : _b;
}

template <typename _T, typename _Compare>
static inline const _T& max(const _T& _a, const _T& _b, _Compare _compare) {
  return _compare(_a, _b) ? _a : _b;
}
#endif
#endif /* !max */

#ifndef sign
#if !defined(__cplusplus)

#define __DEFINE_SIGN(name, T)                                                 \
    static inline T name(T x) { return (x < 0) ? -1 : 1; }

__DEFINE_SIGN(sign_char, char)
__DEFINE_SIGN(sign_schar, signed char)
__DEFINE_SIGN(sign_short, short)
__DEFINE_SIGN(sign_int, int)
__DEFINE_SIGN(sign_long, long)
__DEFINE_SIGN(sign_llong, long long)

#undef __DEFINE_SIGN
#define sign(x)                                                                \
    _Generic(((x) < 0) ? -1 : 1,                                               \
              char: sign_char,                                                 \
       signed char: sign_schar,                                                \
             short: sign_short,                                                \
               int: sign_int,                                                  \
              long: sign_long,                                                 \
         long long: sign_llong,                                                \
         default: "bad type for sign")(x)
#else
static inline constexpr int sign(signed int _x) { return (_x < 0) ? -1 : 1; }

static inline constexpr int sign(signed short _x) { return (_x < 0) ? -1 : 1; }

static inline constexpr int sign(signed long _x) { return (_x < 0) ? -1 : 1; }
#endif
#endif /* !sign */

#ifndef clamp
#if !defined(__cplusplus)
#define clamp(x, y, z)                                                         \
    __extension__ ({                                                           \
       __typeof__ (x) _x = (x);                                                \
       __typeof__ (y) _y = (y);                                                \
       __typeof__ (z) _z = (z);                                                \
       (_x <= _y) ? _y : ((_x >= _z) ? _z : _x);                               \
    })
#else
template<typename _T>
constexpr const _T& clamp(const _T& _value, const _T& _min, const _T& _max) {
  return (_value < _min) ? _min : (_max < _value) ? _max : _value;
}

template<typename _T, typename _Comparer>
constexpr const _T&
clamp(const _T& _value, const _T& _min, const _T& _max, _Comparer _compare) {
  return _compare(_value, _min) ? _min : _compare(_max, _value) ? _max : _value;
}
#endif
#endif /* !clamp */

#ifndef abs
#if !defined(__cplusplus)
#define __DEFINE_ABS(name, T)                                                  \
    static inline T name(T a) { return (a < 0) ? -a : 0; }

__DEFINE_ABS(abs_char, char)
__DEFINE_ABS(abs_schar, signed char)
__DEFINE_ABS(abs_short, short)
__DEFINE_ABS(abs_int, int)
__DEFINE_ABS(abs_long, long)
__DEFINE_ABS(abs_llong, long long)

#undef __DEFINE_ABS

#define abs(a)                                                                 \
    _Generic(((a) < 0) ? -(a) : (a),                                           \
              char: abs_char,                                                  \
       signed char: abs_schar,                                                 \
             short: abs_short,                                                 \
               int: abs_int,                                                   \
              long: abs_long,                                                  \
         long long: abs_llong,                                                 \
         default: "bad type for abs")(a)
#else
static inline char abs(char _a) { return (_a < 0) ? -_a : _a; }

static inline signed char abs(signed char _a) { return (_a < 0) ? -_a : _a; }

static inline short abs(short _a) { return (_a < 0) ? -_a : _a; }

static inline int abs(int _a) { return (_a < 0) ? -_a : _a; }

static inline long abs(long _a) { return __builtin_labs(_a); }

static inline long long abs(long long _a) { return __builtin_llabs(_a); }
#endif
#endif /* !abs */

#endif /* !_YAUL_GAMEMATH_DEFS_H_ */

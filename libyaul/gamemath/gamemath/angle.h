#ifndef _YAUL_GAMEMATH_ANGLE_H_
#define _YAUL_GAMEMATH_ANGLE_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <gamemath/defs.h>
#include <gamemath/fix16.h>

/// @addtogroup MATH
/// @defgroup MATH_ANGLE Angle
/// @ingroup MATH
/// @{

#if !defined(__cplusplus)
/// @brief Not yet documented.
///
/// @param d Not yet documented.
#define RAD2ANGLE(d) ((angle_t)((65536.0 * (d)) / (2.0 * M_PI)))

/// @brief Not yet documented.
///
/// @param d Not yet documented.
#define DEG2ANGLE(d) ((angle_t)((65536.0 * (d)) / 360.0))
#endif /* !__cplusplus */

#if !defined(__cplusplus)
/// @brief Not yet documented.
typedef int16_t angle_t;

/// @brief Not yet documented.
typedef struct euler {
    /// @brief Not yet documented.
    angle_t pitch;
    /// @brief Not yet documented.
    angle_t yaw;
    /// @brief Not yet documented.
    angle_t roll;
} euler_t;
#else
/// @brief Not yet documented.
struct angle_t {
    /// @brief Not yet documented.
    int16_t value;

    angle_t() = default;

    inline const angle_t& operator+(const angle_t& other) const;
    inline const angle_t& operator-(const angle_t& other) const;

    inline angle_t& operator+=(angle_t rhs);
    inline angle_t& operator-=(angle_t rhs);

    inline bool operator<(const angle_t& other) const;
    inline bool operator<(int32_t other) const;
    inline bool operator>(const angle_t& other) const;
    inline bool operator>(int32_t other) const;
    inline bool operator<=(const angle_t& other) const;
    inline bool operator<=(int32_t other) const;
    inline bool operator>=(const angle_t& other) const;
    inline bool operator>=(int32_t other) const;
    inline bool operator==(const angle_t& other) const;
    inline bool operator==(int32_t other) const;

    static constexpr inline angle_t from_rad_double(double rad);

    static constexpr inline angle_t from_deg_double(double value);

    inline angle_t from_deg(fix16_t degree);

    inline angle_t from_rad(fix16_t rad);

    inline fix16_t to_fix16() const;

    inline void to_string(char* buffer, int32_t decimals = 7);
};

static_assert(sizeof(angle_t) == 2);

/// @brief Not yet documented.
struct euler_t {
    /// @brief Not yet documented.
    angle_t pitch;
    /// @brief Not yet documented.
    angle_t yaw;
    /// @brief Not yet documented.
    angle_t roll;
};
#endif /* !__cplusplus */

#if !defined(__cplusplus)
/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Not yet documented.
static inline fix16_t __always_inline
angle_deg_to(angle_t angle)
{
    return (angle * 360);
}

/// @brief Not yet documented.
///
/// @param angle Not yet documented.
///
/// @returns Not yet documented.
static inline fix16_t __always_inline
angle_rad_to(angle_t angle)
{
    return fix16_mul(angle, FIX16(2.0 * M_PI));
}
#else
constexpr angle_t operator""_deg(long double v) { return angle_t::from_deg_double(v); }

constexpr angle_t operator""_rad(long double v) { return angle_t::from_rad_double(v); }

inline const angle_t& angle_t::operator+(const angle_t& other) const { return angle_t{value + other.value}; }
inline const angle_t& angle_t::operator-(const angle_t& other) const { return angle_t{value - other.value}; }

inline angle_t& angle_t::operator+=(angle_t rhs) {
    *this += rhs;
    return *this;
}

inline angle_t& angle_t::operator-=(angle_t rhs) {
    *this -= rhs;
    return *this;
}

inline bool angle_t::operator<(const angle_t& other) const { return value < other.value; }
inline bool angle_t::operator<(int32_t other) const { return value < other; }
inline bool angle_t::operator>(const angle_t& other) const { return value > other.value; }
inline bool angle_t::operator>(int32_t other) const { return value > other; }
inline bool angle_t::operator<=(const angle_t& other) const { return value <= other.value; }
inline bool angle_t::operator<=(int32_t other) const { return value <= other; }
inline bool angle_t::operator>=(const angle_t& other) const { return value >= other.value; }
inline bool angle_t::operator>=(int32_t other) const { return value >= other; }
inline bool angle_t::operator==(const angle_t& other) const { return value == other.value; }
inline bool angle_t::operator==(int32_t other) const { return value == other; }

inline angle_t angle_t::from_deg(fix16_t degree) {
    constexpr fix16_t scale{fix16_t::from_double(1.0 / 360.0)};

    // Drop the fractional part of the fixed value
    return {fix16_t{fix16_int32_mul(scale, degree)}.fractional()};
}

inline angle_t angle_t::from_rad(fix16_t rad) {
    constexpr fix16_t scale{fix16_t::from_double(1.0 / M_PI)};

    // Drop the fractional part of the fixed value
    return {fix16_t{fix16_int32_mul(scale, rad)}.fractional()};
}

constexpr inline angle_t angle_t::from_rad_double(double rad) { return angle_t{fix16_t::from_double(rad / M_PI).fractional()}; }

constexpr inline angle_t angle_t::from_deg_double(double value) {
    return angle_t{fix16_t::from_double(value / 360.0).fractional()};
}

// TODO: Should we have this? Maybe go via an explicit cast?
inline fix16_t angle_t::to_fix16() const { return fix16_t{static_cast<int32_t>(value)}; }

inline void angle_t::to_string(char* buffer, int32_t decimals) { to_fix16().to_string(buffer, decimals); }
#endif /* !__cplusplus */

/// @}

#endif /* !_YAUL_GAMEMATH_ANGLE_H_ */

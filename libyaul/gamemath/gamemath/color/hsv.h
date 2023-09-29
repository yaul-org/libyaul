/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_COLOR_HSV_H_
#define _YAUL_GAMEMATH_COLOR_HSV_H_

#include <gamemath/color.h>

/// @addtogroup MATH_COLOR
/// @defgroup MATH_COLOR_HSV HSV
/// @ingroup MATH_COLOR
/// @{

/// @brief Not yet documented.
union hsv {
    struct {
        /// @brief Not yet documented.
        uint8_t v;
        /// @brief Not yet documented.
        uint8_t s;
        /* XXX: Hue is in the range of [0..255] instead of [0..360] */
        /// @brief Not yet documented.
        uint8_t h;
    };

    /// @brief Not yet documented.
    uint8_t comp[3];
} __aligned(4);

/// @brief Not yet documented.
///
/// @param _h Not yet documented.
/// @param _s Not yet documented.
/// @param _v Not yet documented.
#define HSV_INITIALIZER(_h, _s, _v)                                            \
{                                                                              \
    {                                                                          \
        _v,                                                                    \
        _s,                                                                    \
        _h                                                                     \
    }                                                                          \
}

/// @brief Not yet documented.
///
/// @param _h Not yet documented.
/// @param _s Not yet documented.
/// @param _v Not yet documented.
#define HSV(_h, _s, _v)                                                        \
    ((hsv_t)HSV_INITIALIZER(_h, _s, _v))

/// @brief Not yet documented.
///
/// @param[in]  color_888 Not yet documented.
/// @param[out] result    Not yet documunted.
extern void rgb888_hsv_conv(const rgb888_t *color_888, hsv_t *result);

/// @}

#endif /* !_YAUL_GAMEMATH_COLOR_HSV_H_ */

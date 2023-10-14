/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GAMEMATH_COLOR_RGB1555_H_
#define _YAUL_GAMEMATH_COLOR_RGB1555_H_

#include <gamemath/color.h>

/// @addtogroup MATH_COLOR
/// @defgroup MATH_COLOR_RGB1555 RGB1555
/// @ingroup MATH_COLOR
/// @{

/// @brief Not yet documented.
union rgb1555 {
    struct {
        /// @brief Not yet documented.
        unsigned int msb:1;
        /// @brief Not yet documented.
        unsigned int b:5;
        /// @brief Not yet documented.
        unsigned int g:5;
        /// @brief Not yet documented.
        unsigned int r:5;
    } __packed;

    /// @brief Not yet documented.
    uint16_t raw;
} __aligned(2);

/// @brief Not yet documented.
///
/// @param _msb Not yet documented.
/// @param _r   Not yet documented.
/// @param _g   Not yet documented.
/// @param _b   Not yet documented.
#define RGB1555_INITIALIZER(_msb, _r, _g, _b)                                  \
{                                                                              \
    {                                                                          \
        _msb,                                                                  \
        _b,                                                                    \
        _g,                                                                    \
        _r                                                                     \
    }                                                                          \
}

/// @brief Not yet documented.
///
/// @param _msb Not yet documented.
/// @param _r   Not yet documented.
/// @param _g   Not yet documented.
/// @param _b   Not yet documented.
#define RGB1555(_msb, _r, _g, _b)                                              \
    ((rgb1555_t)RGB1555_INITIALIZER(_msb, _r, _g, _b))

/// @brief Not yet documented.
///
/// @param _msb Not yet documented.
/// @param _r   Not yet documented.
/// @param _g   Not yet documented.
/// @param _b   Not yet documented.
#define RGB888_RGB1555_INITIALIZER(_msb, _r, _g, _b)                           \
    RGB1555_INITIALIZER(_msb, (_r) >> 3, (_g) >> 3, (_b) >> 3)

/// @brief Not yet documented.
///
/// @param _msb Not yet documented.
/// @param _r   Not yet documented.
/// @param _g   Not yet documented.
/// @param _b   Not yet documented.
#define RGB888_RGB1555(_msb, _r, _g, _b)                                       \
    ((rgb1555_t)RGB888_RGB1555_INITIALIZER(_msb, _r, _g, _b))

/// @brief Not yet documented.
///
/// @param[in]  color_888 Not yet documented.
/// @param[out] result    Not yet documunted.
extern void rgb888_rgb1555_conv(const rgb888_t *color_888, rgb1555_t *result);

/// @}

#endif /* !_YAUL_GAMEMATH_COLOR_RGB1555_H_ */

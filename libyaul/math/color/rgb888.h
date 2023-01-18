/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_COLOR_H_
#error "Header file must not be directly included"
#endif /* !_YAUL_MATH_COLOR_H_ */

/// @addtogroup MATH_COLOR
/// @defgroup MATH_COLOR_RGB888 RGB888
/// @ingroup MATH_COLOR
/// @{

/// @brief Not yet documented.
union rgb888 {
        struct {
                /// @brief Not yet documented.
                unsigned int cc:1;
                unsigned int :7;
                /// @brief Not yet documented.
                unsigned int b:8;
                /// @brief Not yet documented.
                unsigned int g:8;
                /// @brief Not yet documented.
                unsigned int r:8;
        } __packed;

        /// @brief Not yet documented.
        uint8_t comp[3];
        /// @brief Not yet documented.
        uint32_t raw;
} __aligned(4);

/// @brief Not yet documented.
///
/// @param _cc Not yet documented.
/// @param _r  Not yet documented.
/// @param _g  Not yet documented.
/// @param _b  Not yet documented.
#define RGB888_INITIALIZER(_cc, _r, _g, _b)                                    \
    {                                                                          \
            {                                                                  \
                _cc,                                                           \
                _b,                                                            \
                _g,                                                            \
                _r                                                             \
            }                                                                  \
    }

/// @brief Not yet documented.
///
/// @param _cc Not yet documented.
/// @param _r  Not yet documented.
/// @param _g  Not yet documented.
/// @param _b  Not yet documented.
#define RGB888(_cc, _r, _g, _b)                                                \
    ((rgb888_t)RGB888_INITIALIZER(_cc, _r, _g, _b))

/// @brief Not yet documented.
///
/// @param _msb Not yet documented.
/// @param _r   Not yet documented.
/// @param _g   Not yet documented.
/// @param _b   Not yet documented.
/// @param _cc  Not yet documunted.
#define RGB1555_RGB888_INITIALIZER(_msb, _r, _g, _b, _cc)                      \
    RGB888_INITIALIZER(_cc, ((_r) << 3) | ((_r) & 7),                          \
                             ((_g) << 3) | ((_g) & 7),                         \
                             ((_b) << 3) | ((_b) & 7))

/// @brief Not yet documented.
///
/// @param _msb Not yet documented.
/// @param _r   Not yet documented.
/// @param _g   Not yet documented.
/// @param _b   Not yet documented.
/// @param _cc  Not yet documented.
#define RGB1555_RGB888(_msb, _r, _g, _b, _cc)                                  \
    ((rgb888_t)RGB1555_RGB888_INITIALIZER(_msb, _r, _g, _b, _cc))

/// @brief Not yet documented.
///
/// @param[in]  color_1555 Not yet documented.
/// @param[out] result     Not yet documunted.
extern void rgb1555_rgb888_conv(const rgb1555_t *color_1555, rgb888_t *result);

/// @}

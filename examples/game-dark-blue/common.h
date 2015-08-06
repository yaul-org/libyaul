/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef COMMON_H
#define COMMON_H

#define RGB888_TO_RGB555(r, g, b) (0x8000 | (((b) >> 3) << 10) |               \
    (((g) >> 3) << 5) | ((r) >> 3))

#define MATH_SIN(x)     (lut_sin[(x) & 0x1FF])
#define MATH_COS(x)     (lut_cos[(x) & 0x1FF])

#define FIX16_VERTEX3(x, y, z)                                                 \
{                                                                              \
            {                                                                  \
                    F16((x)), F16((y)), F16((z)))                              \
            }                                                                  \
}

#define FIX16_VERTEX4(x, y, z, w)                                              \
{                                                                              \
            {                                                                  \
                    F16((x)), F16((y)), F16((z)), F16((w))                     \
            }                                                                  \
}

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   224

#endif /* !COMMON_H */

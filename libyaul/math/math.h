#ifndef _MATH_H_
#define _MATH_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "uint32.h"
#include "int64.h"
#include "fract32.h"
#include "fix16.h"
#include "int16.h"
#include "color.h"
#include "fix.h"

#ifndef min
#define min(a, b)                                                              \
        ({ __typeof__ (a) _a = (a);                                            \
           __typeof__ (b) _b = (b);                                            \
           (_a < _b) ? _a : _b;                                                \
        })
#endif /* !min */

#ifndef max
#define max(a, b)                                                              \
        ({ __typeof__ (a) _a = (a);                                            \
           __typeof__ (b) _b = (b);                                            \
           (_a > _b) ? _a : _b;                                                \
        })
#endif /* !max */

#ifndef sign
#define sign(x)                                                                \
        ({ __typeof__ (x) _x = (x);                                            \
           (_x < 0) ? -1 : 1;                                                  \
        })
#endif /* !sign */

#ifndef clamp
#define clamp(x, y, z)                                                         \
        ({ __typeof__ (x) _x = (x);                                            \
           __typeof__ (y) _y = (y);                                            \
           __typeof__ (z) _z = (z);                                            \
           (_x <= _y) ? _y : ((_x >= _z) ? _z : _x);                           \
        })
#endif /* !clamp */

static __inline uint32_t
dlog2(uint32_t value)
{
        uint32_t l;

        l = 0;

        while ((value >> l) > 1) {
                ++l;
        }

        return l;
}

static inline uint32_t __always_inline
pow2(uint32_t value)
{
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value++;

        return value;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_MATH_H_ */

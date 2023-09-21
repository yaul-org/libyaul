#ifndef _YAUL_GAMEMATH_DEFS_H_
#define _YAUL_GAMEMATH_DEFS_H_

#include <stdint.h>

#define M_PI       (3.14159265358979323846f) /* pi */
#define M_PI_2     (1.57079632679489661923f) /* pi/2 */
#define M_PI_4     (0.78539816339744830962f) /* pi/4 */
#define M_1_PI     (0.31830988618379067154f) /* 1/pi */
#define M_2_PI     (0.63661977236758134308f) /* 2/pi */
#define M_2_SQRTPI (1.12837916709551257390f) /* 2/sqrt(pi) */
#define M_SQRT2    (1.41421356237309504880f) /* sqrt(2) */
#define M_SQRT1_2  (0.70710678118654752440f) /* 1/sqrt(2) */

#ifndef min
#define min(a, b)                                                              \
    __extension__ ({ __typeof__ (a) _a = (a);                                  \
       __typeof__ (b) _b = (b);                                                \
       (_a < _b) ? _a : _b;                                                    \
    })
#endif /* !min */

#ifndef max
#define max(a, b)                                                              \
    __extension__ ({ __typeof__ (a) _a = (a);                                  \
       __typeof__ (b) _b = (b);                                                \
       (_a > _b) ? _a : _b;                                                    \
    })
#endif /* !max */

#ifndef sign
#define sign(x)                                                                \
    __extension__ ({ __typeof__ (x) _x = (x);                                  \
       (_x < 0) ? -1 : 1;                                                      \
    })
#endif /* !sign */

#ifndef clamp
#define clamp(x, y, z)                                                         \
    __extension__ ({ __typeof__ (x) _x = (x);                                  \
       __typeof__ (y) _y = (y);                                                \
       __typeof__ (z) _z = (z);                                                \
       (_x <= _y) ? _y : ((_x >= _z) ? _z : _x);                               \
    })
#endif /* !clamp */

#endif /* !_YAUL_GAMEMATH_DEFS_H_ */

#ifndef _MATH_H_
#define _MATH_H_

#include <sys/cdefs.h>

#include "uint32.h"
#include "fix16.h"
#include "int16.h"
#include "color.h"
#include "fix.h"

__BEGIN_DECLS

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

__END_DECLS

#endif /* !_MATH_H_ */

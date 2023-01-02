#ifndef _YAUL_MATH_ANGLE_H_
#define _YAUL_MATH_ANGLE_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

#define RAD2ANGLE(d) ((angle_t)((65536.0f * (d)) / (2 * M_PI)))
#define DEG2ANGLE(d) ((angle_t)((65536.0f * (d)) / 360.0f))

typedef int16_t angle_t;

static inline fix16_t __always_inline
angle_deg_to(angle_t angle)
{
        return (angle * 360);
}

static inline fix16_t __always_inline
angle_rad_to(angle_t angle)
{
        return fix16_mul(angle, FIX16_2PI);
}

__END_DECLS

#endif /* !_YAUL_MATH_ANGLE_H_ */

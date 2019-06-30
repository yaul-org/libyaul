#include <stdbool.h>

#include "fix16.h"

static inline fix16_t __always_inline
_fix16_rs(fix16_t x)
{
        return (x >> 1);
}

/* This assumes that the input value is >= 1.
 *
 * Note that this is only ever called with inValue >= 1 (because it has a
 * wrapper to check. As such, the result is always less than the input. */
static fix16_t
_fix16_log2_inner(fix16_t x)
{
        fix16_t result = 0;

        while (x >= fix16_int32_from(2)) {
                result++;
                x = _fix16_rs(x);
        }

        if (x == 0) {
                return (result << 16);
        }

        uint_fast8_t i;

        for (i = 16; i > 0; i--) {
                x = fix16_mul(x, x);
                result <<= 1;

                if (x >= fix16_int32_from(2)) {
                        result |= 1;
                        x = _fix16_rs(x);
                }
        }


        return result;
}

/* Calculates the log base 2 of input. Note that negative inputs are invalid!
 * (will return fix16_overflow, since there are no exceptions).
 *
 * For example, 2 to the power output = input.
 *
 * It's equivalent to the log or ln functions, except it uses base 2 instead of
 * base 10 or base e. This is useful as binary things like this are easy for
 * binary devices, like modern microprocessros, to calculate.
 *
 * This can be used as a helper function to calculate powers with non-integer
 * powers and/or bases. */
fix16_t
fix16_log2(fix16_t x)
{
        /* Note that a negative x gives a non-real result.
         *
         * If x == 0, the limit of log2(x) as x -> 0 = -infinity.
         *
         * log2(-ve) gives a complex result. */
        if (x <= 0) {
                return fix16_overflow;
        }

        /* If the input is less than one, the result is -log2(1.0 / in) */
        if (x < FIX16_ONE) {
                /* Note that the inverse of this would overflow. This is the
                 * exact answer for log2(1.0 / 65536) */
                if (x == 1) {
                        return fix16_int32_from(-16);
                }

                fix16_t inverse = fix16_div(FIX16_ONE, x);
                return -_fix16_log2_inner(inverse);
        }

        /* If input >= 1, just proceed as normal. Note that x == FIX16_ONE is a
         * special case, where the answer is 0. */
        return _fix16_log2_inner(x);
}

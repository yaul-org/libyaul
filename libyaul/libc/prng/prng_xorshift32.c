/*-
 * CC BY-SA 3.0 <https://en.wikipedia.org/wiki/Xorshift>
 */

#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t a;
} xorshift32_state_t;

static xorshift32_state_t _state;

void
prng_xorshift32_seed(uint32_t seed)
{
    _state.a = seed;
}

uint32_t
prng_xorshift32(void)
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint32_t x;
    x = _state.a;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    _state.a = x;

    return x;
}

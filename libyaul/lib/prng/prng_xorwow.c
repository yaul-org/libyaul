/*-
 * CC BY-SA 3.0 <https://en.wikipedia.org/wiki/Xorshift>
 */

#include <stdlib.h>
#include <stdint.h>

typedef struct xorwow_state {
        uint32_t x[5];
        uint32_t counter;
} xorwow_state_t;

static xorwow_state_t _state;

void
prng_xorwow_seed(uint32_t s)
{
        _state.x[0] = s;
        _state.x[1] = 0;
        _state.x[2] = 0;
        _state.x[3] = 0;
        _state.x[4] = 0;

        _state.counter = 0;
}

/* The state array must be initialized to not be all zero in the first four words */
uint32_t
prng_xorwow()
{
        /* Algorithm "xorwow" from p. 5 of Marsaglia, "Xorshift RNGs" */
        uint32_t t  = _state.x[4];

        const uint32_t s  = _state.x[0];  /* Perform a contrived 32-bit shift */
        _state.x[4] = _state.x[3];
        _state.x[3] = _state.x[2];
        _state.x[2] = _state.x[1];
        _state.x[1] = s;

        t ^= t >> 2;
        t ^= t << 1;
        t ^= s ^ (s << 4);
        _state.x[0] = t;
        _state.counter += 362437;

        return (t + _state.counter);
}

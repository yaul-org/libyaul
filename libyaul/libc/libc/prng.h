#ifndef _LIB_PRNG_H_
#define _LIB_PRNG_H_

#include <stdint.h>

#include <sys/cdefs.h>

typedef uint8_t crc_t;

__BEGIN_DECLS

extern void prng_xorshift32_seed(uint32_t seed);
extern uint32_t prng_xorshift32(void);

extern void prng_xorwow_seed(uint32_t seed);
extern uint32_t prng_xorwow(void);

__END_DECLS

#endif /* _LIB_PRNG_H_ */

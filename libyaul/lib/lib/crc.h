#ifndef _LIB_CRC_H_
#define _LIB_CRC_H_

#include <stdint.h>

#include <sys/cdefs.h>

typedef uint8_t crc_t;

__BEGIN_DECLS

extern crc_t crc_calculate(const uint8_t *, size_t);

__END_DECLS

#endif /* _LIB_CRC_H_ */

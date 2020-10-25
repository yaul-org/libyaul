/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef CRC_H_
#define CRC_H_

#include <inttypes.h>
#include <stdlib.h>

typedef uint8_t crc_t;

crc_t crc_calculate(const uint8_t *, size_t);

#endif /* !CRC_H_ */

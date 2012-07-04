/*
 * See LICENSE for details.
 */

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#include <inttypes.h>

static inline uint16_t
be16dec(const void *buf)
{
        const uint8_t *p = (const uint8_t *)buf;

        return (uint16_t)((p[0] << 8) | p[1]);
}

static inline uint16_t
le16dec(const void *buf)
{
        const uint8_t *p = (const uint8_t *)buf;

        return (uint16_t)((p[1] << 8) | p[0]);
}

static inline uint32_t
be32dec(const void *buf)
{
        const uint8_t *p = (const uint8_t *)buf;

        return ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

static inline uint32_t
le32dec(const void *buf)
{
        const uint8_t *p = (const uint8_t *)buf;

        return ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
}

#endif /* !_ENDIAN_H_ */

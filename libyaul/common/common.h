/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _COMMON_H_
#define _COMMON_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static __inline__ uint32_t
common_log2_down(uint32_t value)
{
        uint32_t l;

        l = 0;
        while ((value >> l) > 1)
                ++l;

        return l;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_COMMON_H_ */

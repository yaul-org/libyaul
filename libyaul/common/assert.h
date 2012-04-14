/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <inttypes.h>

void panic(const char *, const char *, const char *, uint32_t, const char *);

#define assert(c, s) do {                                                       \
                if (!(c)) {                                                     \
                        panic(s, #c, __FILE__, __LINE__, __FUNCTION__);         \
                }                                                               \
        } while (0)

#endif /* !_ASSERT_H_ */

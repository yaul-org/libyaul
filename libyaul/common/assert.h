/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <inttypes.h>

#include <stack.h>

void _assert(const char *, const char *, const char *, uint32_t, const char *,
    const char *);

#define assert(c, s) do {                                                       \
                if (!(c)) {                                                     \
                        /*
                         * KLUDGE
                         *
                         * Because GCC is too fucking retarded and just
                         * refuses to give me a proper frame-pointer, I
                         * have to start the backtrace here
                         */                                                     \
                        char *_bt;                                              \
                        _bt = stack_backtrace();                                \
                        _assert(s, #c, __FILE__, __LINE__, __FUNCTION__, _bt);  \
                }                                                               \
        } while (0)

#endif /* !_ASSERT_H_ */

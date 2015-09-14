/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG   1

#ifdef DEBUG
#include <stdio.h>
#include <stdbool.h>

#define DEBUG_PRINTF(fmt, ...) do {                                            \
        (void)fprintf(stderr, "%s():L%i:" " " fmt, __FUNCTION__, __LINE__,     \
            ##__VA_ARGS__);                                                    \
} while(false)
#else
#define DEBUG_PRINTF(x...)
#endif /* DEBUG */

#endif /* !DEBUG_H_ */

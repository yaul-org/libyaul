/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _ARP_INTERNAL_H_
#define _ARP_INTERNAL_H_

#include <scu-internal.h>

/* Macros specific for processor */
#define ARP(x)          (0x22000000 + (x))

/* Helpers specific to this processor */
#define USER_VECTOR(x)  ((x))
#define USER_VECTOR_CALL(x) do {                                              \
        __asm__ __volatile__ ("trapa #" # x "\n");                            \
} while (false)

#define OUTPUT          0x00080001
#define STATUS          0x00100001
#define INPUT           0x00180001
#define VERSION         0x00004AF0

/* ARP user callback */
extern arp_callback_t arp_callback;

#endif /* !_ARP_INTERNAL_H_ */

/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _ARP_INTERNAL_H_
#define _ARP_INTERNAL_H_

#include <scu/map.h>
#include <arp/map.h>

/* Helpers specific to this processor */
#define USER_VECTOR(x)  ((x))
#define USER_VECTOR_CALL(x) do {                                               \
        __asm__ __volatile__ ("trapa #" # x "\n");                             \
} while (false)

/* ARP user callback */
extern arp_callback_t arp_callback;

#endif /* !_ARP_INTERNAL_H_ */

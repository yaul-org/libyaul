/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _ARP_H__
#define _ARP_H__

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
        void *ptr;      /* Address */
        size_t len;     /* Length of transfer */
        uint8_t function; /* Function recently executed */
} __attribute__ ((aligned (4))) arp_callback_t;

extern bool arp_busy_status(void);
extern bool arp_sync_nonblock(void);
extern uint32_t arp_read_long(void);
extern uint8_t arp_read_byte(void);
extern uint8_t arp_xchg_byte(uint8_t);
extern void arp_function_callback(void (*)(arp_callback_t *));
extern void arp_function_nonblock(void);
extern void arp_send_long(uint32_t);
extern void arp_sync(void);

#endif /* !_ARP_H_ */

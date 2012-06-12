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

extern bool arp_busy_status(void);
extern uint32_t arp_read_long(void);
extern uint8_t arp_read_byte(void);
extern uint8_t arp_xchg_byte(uint8_t);
extern void arp_sync(void);
extern void arp_send_long(uint32_t);
extern void arp_function(void);

#endif /* !_ARP_H_ */

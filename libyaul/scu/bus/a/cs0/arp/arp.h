/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _ARP_H_
#define _ARP_H_

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

typedef struct arp_callback arp_callback_t;

typedef void (*arp_callback_handler)(const arp_callback_t *);

struct arp_callback {
        uint8_t function; /* Function recently executed */
        void *ptr;        /* Address */
        bool exec;        /* Whether or not it's executable */
        size_t len;       /* Length of transfer */
} __aligned(4);

extern bool arp_busy_status(void);
extern bool arp_sync_nonblock(void);
extern char *arp_version_get(void);
extern uint32_t arp_long_read(void);
extern uint8_t arp_byte_read(void);
extern uint8_t arp_byte_xchg(uint8_t);
extern void arp_function_callback_set(arp_callback_handler);
extern void arp_function_nonblock(void);
extern void arp_return(void) __noreturn;
extern void arp_long_send(uint32_t);
extern void arp_sync(void);

__END_DECLS

#endif /* !_ARP_H_ */

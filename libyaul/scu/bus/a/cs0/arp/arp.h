/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _ARP_H__
#define _ARP_H__

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
        uint8_t function; /* Function recently executed */
        void *ptr;      /* Address */
        bool exec;      /* Whether or not it's executable */
        size_t len;     /* Length of transfer */
} __attribute__ ((aligned (4))) arp_callback_t;

extern bool arp_busy_status(void);
extern bool arp_sync_nonblock(void);
extern char *arp_version(void);
extern uint32_t arp_read_long(void);
extern uint8_t arp_read_byte(void);
extern uint8_t arp_xchg_byte(uint8_t);
extern void arp_function_callback(void (*)(arp_callback_t *));
extern void arp_function_nonblock(void);
extern void arp_send_long(uint32_t);
extern void arp_sync(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_ARP_H_ */

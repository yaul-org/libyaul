/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>
#include <cpu/intc.h>

#include <assert.h>
#include <string.h>

#include "arp-internal.h"

arp_callback_t arp_callback;
static void (*arp_cb)(arp_callback_t *) = NULL;

static void arp_trampoline(void);

void
arp_function_callback(void (*cb)(arp_callback_t *))
{
        /* Disable interrupts */
        cpu_intc_disable();

        /* Clear ARP callback */
        memset(&arp_callback.ptr, 0x00, sizeof(arp_callback));

        assert(cb != NULL);
        arp_cb = cb;
        cpu_intc_interrupt_set(USER_VECTOR(32), arp_trampoline);

        /* Enable interrupts */
        cpu_intc_enable();
}

static void
arp_trampoline(void)
{

        arp_cb(&arp_callback);

        /* Clear ARP user callback */
        arp_callback.function = 0x00;
        arp_callback.ptr = NULL;
        arp_callback.exec = false;
        arp_callback.len = 0;
}

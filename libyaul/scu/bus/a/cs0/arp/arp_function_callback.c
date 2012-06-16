/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <arp.h>
#include <cpu/intc.h>

#include <assert.h>
#include <string.h>

#include "arp_internal.h"

arp_callback_t arp_callback;
void (*arp_cb)(arp_callback_t *) = NULL;

void
arp_function_callback(void (*cb)(arp_callback_t *))
{
        /* Disable interrupts */
        cpu_intc_disable();

        /* Clear ARP callback */
        memset(&arp_callback.ptr, 0x00, sizeof(arp_callback));

        assert(cb != NULL);
        arp_cb = cb;
        cpu_intc_interrupt_set(USER_VECTOR(32), (uint32_t)&arp_function_trampoline);

        /* Enable interrupts */
        cpu_intc_enable();
}

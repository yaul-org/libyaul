/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>

#include <arp.h>
#include <cpu/intc.h>

#include "arp-internal.h"

struct arp_callback arp_callback;

static void (*arp_cb)(const struct arp_callback *) = NULL;

static void _arp_trampoline(void);

void
arp_function_callback_set(void (*cb)(const struct arp_callback *))
{
        /* Disable interrupts */
        uint32_t sr_mask;
        sr_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        /* Clear ARP callback */
        memset(&arp_callback.ptr, 0x00, sizeof(arp_callback));

        assert(cb != NULL);
        arp_cb = cb;
        cpu_intc_ihr_set(32, _arp_trampoline);

        /* Enable interrupts */
        cpu_intc_mask_set(sr_mask);
}

static void
_arp_trampoline(void)
{
        arp_cb(&arp_callback);

        /* Clear ARP user callback */
        arp_callback.function = 0x00;
        arp_callback.ptr = NULL;
        arp_callback.exec = false;
        arp_callback.len = 0;
}

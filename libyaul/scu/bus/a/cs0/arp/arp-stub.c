/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "arp.h"

bool
arp_busy_status(void)
{
        return false;
}

void
arp_function_callback(void (*cb)(arp_callback_t *))
{
}

void
arp_function_nonblock(void)
{
}

uint8_t
arp_read_byte(void)
{
        return 0x00;
}

uint32_t
arp_read_long(void)
{
        return 0x00000000;
}

void __noreturn
arp_return(void)
{
}

void
arp_send_long(uint32_t w)
{
}

void
arp_sync(void)
{
}

bool
arp_sync_nonblock(void)
{
}

char *
arp_version(void)
{
        return NULL;
}

uint8_t
arp_xchg_byte(uint8_t c)
{
        return 0x00;
}

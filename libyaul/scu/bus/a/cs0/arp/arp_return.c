/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>

#include <sys/cdefs.h>

#include "arp-internal.h"

void __noreturn
arp_return(void)
{
        ((void (*)(void))0x02000100)();

        while (true) {
        }
}

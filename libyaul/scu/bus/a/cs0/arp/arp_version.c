/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <arp.h>
#include <cpu/intc.h>

#include "arp-internal.h"

char *
arp_version(void)
{
        const char *arp_ver;
        char *buf;
        size_t ver_len;

        arp_ver = (const char *)ARP(VERSION);
        ver_len = 255;
        if ((buf = (char *)malloc(ver_len + 1)) == NULL) {
                return NULL;
        }
        memset(buf, '\0', ver_len);
        memcpy(buf, arp_ver, ver_len);
        buf[ver_len] = '\0';

        return buf;
}

/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _ARP_MAP_H_
#define _ARP_MAP_H_

/* Macros specific for processor */
#define ARP(x)  (0x22000000UL + (x))

#define OUTPUT  0x00080001UL
#define STATUS  0x00100001UL
#define INPUT   0x00180001UL
#define VERSION 0x00004AF0UL

#endif /* !_ARP_MAP_H_ */

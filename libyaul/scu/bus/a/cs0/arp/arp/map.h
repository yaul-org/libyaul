/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _ARP_MAP_H_
#define _ARP_MAP_H_

/* Macros specific for processor */
#define ARP(x)          (0x22000000 + (x))

#define OUTPUT          0x00080001
#define STATUS          0x00100001
#define INPUT           0x00180001
#define VERSION         0x00004AF0

#endif /* !_ARP_MAP_H_ */

/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SMPC_INTERNAL_H_
#define _SMPC_INTERNAL_H_

#include <scu-internal.h>

#include <smpc/map.h>
#include <smpc/peripheral.h>
#include <smpc/rtc.h>

extern smpc_time_t __smpc_time;

extern smpc_peripheral_port_t __smpc_peripheral_ports[];

extern void __smpc_init(void);

#endif /* !_SMPC_INTERNAL_H_ */

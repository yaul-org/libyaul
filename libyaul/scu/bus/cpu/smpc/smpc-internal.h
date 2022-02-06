/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SMPC_INTERNAL_H_
#define _SMPC_INTERNAL_H_

#include <scu-internal.h>

#include <smpc/map.h>

struct smpc_peripheral_port;

extern struct smpc_peripheral_port smpc_peripheral_port_1;
extern struct smpc_peripheral_port smpc_peripheral_port_2;

extern void __smpc_init(void);
extern void __smpc_peripheral_init(void);

#endif /* !_SMPC_INTERNAL_H_ */

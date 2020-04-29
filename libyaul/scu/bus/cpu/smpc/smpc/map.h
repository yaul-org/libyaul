/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SMPC_MAP_H_
#define _SMPC_MAP_H_

#include <scu/map.h>

/* Specific macros */
#define IREG(x)         (0x20100001UL + ((x) << 1))
#define OREG(x)         (0x20100021UL + ((x) << 1))
#define SMPC(x)         (0x20100000UL + (x))

#define SMPC_IREGS      7
#define SMPC_OREGS      32

/* SMPC */
#define COMREG          0x01FUL
#define SR              0x061UL
#define SF              0x063UL
#define PDR1            0x075UL
#define PDR2            0x077UL
#define DDR1            0x079UL
#define DDR2            0x07BUL
#define IOSEL1          0x07DUL
#define IOSEL2          0x07DUL
#define EXLE1           0x07FUL
#define EXLE2           0x07FUL

#endif /* !_SMPC_MAP_H_ */

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
#define IREG(x)         (0x20100001 + ((x) << 1))
#define OREG(x)         (0x20100021 + ((x) << 1))
#define SMPC(x)         (0x20100000 + (x))

#define SMPC_IREGS      7
#define SMPC_OREGS      32

/* SMPC */
#define COMREG          0x01F
#define SR              0x061
#define SF              0x063
#define PDR1            0x075
#define PDR2            0x077
#define DDR1            0x079
#define DDR2            0x07B
#define IOSEL1          0x07D
#define IOSEL2          0x07D
#define EXLE1           0x07F
#define EXLE2           0x07F

#endif /* !_SMPC_MAP_H_ */

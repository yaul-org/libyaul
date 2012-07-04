/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_INTERNAL_H_
#define _SCU_INTERNAL_H_

#include <inttypes.h>
#include <stddef.h>

/* Write and read directly to specified address */
#define MEMORY_WRITE(t, x, y)   (*(volatile uint ## t ## _t *)(x) = (y))
#define MEMORY_READ(t, x)       (*(volatile uint ## t ## _t *)(x))

/* Macros specific for processor */
#define SCU(x)          (0x25FE0000 + (x))
#define CS0(x)          (0x22400000 + (x))
#define CS1(x)          (0x24000000 + (x))
#define DUMMY(x)        (0x25000000 + (x))

/* SCU */
#define D0R     0x0000
#define D0W     0x0004
#define D0C     0x0008
#define D0AD    0x000C
#define D0EN    0x0010
#define D0MD    0x0014
#define D1R     0x0020
#define D1W     0x0024
#define D1C     0x0028
#define D1AD    0x002C
#define D1EN    0x0030
#define D1MD    0x0034
#define D2R     0x0040
#define D2W     0x0044
#define D2C     0x0048
#define D2AD    0x004C
#define D2EN    0x0050
#define D2MD    0x0054
#define DSTA    0x007C
#define PPAF    0x0080
#define PPD     0x0084
#define PDA     0x0088
#define PDD     0x008C
#define T0C     0x0090
#define T1S     0x0094
#define T1MD    0x0098
#define IMS     0x00A0
#define IST     0x00A4
#define AIACK   0x00A8
#define ASR0    0x00B0
#define ASR1    0x00B4
#define AREF    0x00B8
#define RSEL    0x00C4
#define VER     0x00C8

#endif /* !_SCU_INTERNAL_H_ */

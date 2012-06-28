/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _CPU_INTERNAL_H_
#define _CPU_INTERNAL_H_

#include <scu-internal.h>

/* Macros specific for processor. */
#define CPU(x)          (0xFFFFF000 + (x))

#define IPRB            0x0E60
#define VCRA            0x0E62
#define VCRB            0x0E64
#define VCRC            0x0E66
#define VCRD            0x0E68
#define IPRA            0x0EE2
#define VCRWDT          0x0EE4
#define DRCR0           0x0E71
#define DRCR1           0x0E72
#define VCRDIV          0x0F0C
#define BARAH           0x0F40
#define BARAL           0x0F42
#define BAMRAH          0x0F44
#define BAMRAL          0x0F46
#define BBRA            0x0F48
#define BARBH           0x0F60
#define BARBL           0x0F62
#define BAMRBH          0x0F64
#define BAMRBL          0x0F66
#define BBRB            0x0F68
#define BDRBH           0x0F70
#define BDRBL           0x0F72
#define BDMRBH          0x0F74
#define BDMRBL          0x0F76
#define BRCR            0x0F78
#define SAR0            0x0F80
#define DAR0            0x0F84
#define TCR0            0x0F88
#define CHCR0           0x0F8C
#define SAR1            0x0F90
#define DAR1            0x0F94
#define TCR1            0x0F98
#define CHCR1           0x0F9C
#define VCRDMA0         0x0FA0
#define VCRDMA1         0x0FA8
#define DMAOR           0x0FB0

#endif /* !_CPU_INTERNAL_H_ */

/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_INTERNAL_H_
#define _CPU_INTERNAL_H_

#include <scu-internal.h>

#include <cpu/map.h>

extern void __cpu_init(void);
extern void __cpu_divu_init(void);
extern void __cpu_dmac_init(void);

#endif /* !_CPU_INTERNAL_H_ */

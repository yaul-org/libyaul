/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_INTERNAL_H_
#define _SCU_INTERNAL_H_

#include <stdint.h>
#include <stddef.h>

#include <scu/dma.h>
#include <scu/map.h>

extern void __scu_init(void);
extern void __scu_dma_init(void);
extern void __scu_dsp_init(void);
extern void __scu_timer_init(void);

#endif /* !_SCU_INTERNAL_H_ */

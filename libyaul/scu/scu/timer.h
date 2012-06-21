/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _SCU_TIMER_H_
#define _SCU_TIMER_H_

#include <inttypes.h>
#include <stdbool.h>

extern void scu_timer_0_set(uint16_t);
extern void scu_timer_1_mode_clear(void);
extern void scu_timer_1_mode_set(bool);
extern void scu_timer_1_set(uint16_t);

#endif /* !_SCU_TIMER_H_ */

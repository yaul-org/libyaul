/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCU_TIMER_H_
#define _SCU_TIMER_H_

#include <inttypes.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void scu_timer_0_set(uint16_t);
extern void scu_timer_1_mode_clear(void);
extern void scu_timer_1_mode_set(bool);
extern void scu_timer_1_set(uint16_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_SCU_TIMER_H_ */

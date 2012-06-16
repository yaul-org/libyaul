/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _INTC_H_
#define _INTC_H_

#include <inttypes.h>

extern uint32_t cpu_intc_interrupt_get(void);
extern uint32_t cpu_intc_vector_base_get(void);
extern uint8_t cpu_intc_mask_get(void);
extern void cpu_intc_disable(void);
extern void cpu_intc_enable(void);
extern void cpu_intc_interrupt_set(uint32_t, uint32_t);
extern void cpu_intc_mask_set(uint8_t);
extern void cpu_intc_vector_base_set(uint32_t);

#endif /* !_INTC_H */

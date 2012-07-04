/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _INTC_H_
#define _INTC_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void (*cpu_intc_interrupt_get(uint8_t))(void);
extern void (**cpu_intc_vector_base_get(void))(void);
extern uint8_t cpu_intc_mask_get(void);
extern void cpu_intc_disable(void);
extern void cpu_intc_enable(void);
extern void cpu_intc_interrupt_set(uint8_t, void (*)(void));
extern void cpu_intc_mask_set(uint8_t);
extern void cpu_intc_vector_base_set(void (**)(void));

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_INTC_H */

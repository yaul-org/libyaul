/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _INTC_H_
#define _INTC_H_

extern unsigned char    cpu_intc_lvl_get(void);
extern void             cpu_intc_lvl_set(unsigned char);
extern unsigned long    cpu_intc_vct_get(void);
extern void             cpu_intc_vct_set(unsigned long, unsigned long);
extern unsigned long    cpu_intc_vct_base_get(void);
extern void             cpu_intc_vct_base_set(unsigned long);
extern void             cpu_intc_vct_enable(void);
extern void             cpu_intc_vct_disable(void);

#endif /* !_INTC_H */

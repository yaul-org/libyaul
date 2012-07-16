/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _IHR_H_
#define _IHR_H_

extern void ihr_break(void);
extern void ihr_cpu_address_error(void);
extern void ihr_dma_address_error(void);
extern void ihr_illegal_instruction(void);
extern void ihr_illegal_slot(void);
extern void ihr_ubc(void);

#endif /* !_IHR_H_ */

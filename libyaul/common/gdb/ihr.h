/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
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

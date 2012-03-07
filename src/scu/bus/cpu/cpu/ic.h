/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _IC_H_
#define _IC_H_

extern unsigned char    cpu_ic_lvl_get(void);
extern void             cpu_ic_lvl_set(unsigned char);
extern unsigned long    cpu_ic_vct_get(void);
extern void             cpu_ic_vct_set(unsigned long);

#endif /* !_IC_H */

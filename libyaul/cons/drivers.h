/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DRIVERS_H_
#define _DRIVERS_H_

#include <vdp2.h>

#include "cons.h"

void cons_vdp1_init(struct cons *);
void cons_vdp2_init(struct cons *);

#endif /* !_DRIVERS_H_ */

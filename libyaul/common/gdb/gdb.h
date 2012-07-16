/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _GDB_H_
#define _GDB_H_

#include <cpu/registers.h>

#include "gdb-signals.h"
#include "sh2-704x.h"

void gdb_monitor(struct cpu_registers *, int);

#endif /* !_GDB_H_ */

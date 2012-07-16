/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _GDB_H_
#define _GDB_H_

#include <cpu/registers.h>

#include "gdb-signals.h"
#include "sh2-704x.h"

void gdb_monitor(struct cpu_registers *, int);

#endif /* !_GDB_H_ */

/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _GDB_H_
#define _GDB_H_

#include <cpu/registers.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gdb-signals.h"
#include "sh2-704x.h"

void gdb_monitor(struct cpu_registers *, int);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_GDB_H_ */

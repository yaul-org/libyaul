/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _GDB_H_
#define _GDB_H_

#include <cpu/instructions.h>

__BEGIN_DECLS

static inline void __always_inline
gdb_break(void)
{
        cpu_instr_trapa(0x20);
}

extern void gdb_init(void);

__END_DECLS

#endif /* !_GDB_H_ */

/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_GDB_H_
#define _YAUL_GDB_H_

#include <cpu/instructions.h>

__BEGIN_DECLS

#define GDB_TRAPA_VECTOR_NUMBER (32)

static inline void __always_inline
gdb_break(void)
{
        cpu_instr_trapa(GDB_TRAPA_VECTOR_NUMBER);
}

extern void gdb_init(void);

__END_DECLS

#endif /* !_YAUL_GDB_H_ */

/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SH2_704X_H_
#define _SH2_704X_H_

#include <cpu/registers.h>
#include <cpu/instructions.h>

#include <stdbool.h>

__BEGIN_DECLS

static inline void __always_inline
gdb_sync(void)
{
        cpu_instr_trapa(0x20);
}

bool gdb_register_file_read(struct cpu_registers *, uint32_t, uint32_t *);
bool gdb_register_file_write(struct cpu_registers *, uint32_t, uint32_t);
int gdb_break(uint32_t, uint32_t, uint32_t);
int gdb_getc(void);
int gdb_remove_break(uint32_t, uint32_t, uint32_t);
void gdb_kill(void);
void gdb_monitor_entry(struct cpu_registers *);
void gdb_putc(int);
void gdb_sh2_704x_init(void);
void gdb_step(struct cpu_registers *, uint32_t);

__END_DECLS

#endif /* !_SH2_704X_H_ */

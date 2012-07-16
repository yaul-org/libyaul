/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SH2_704X_H_
#define _SH2_704X_H_

#include <cpu/registers.h>

#include <stdbool.h>

#define gdb_break() do {                                                      \
        __asm__ __volatile__ ("trapa #0x20");                                 \
} while (false)

bool gdb_register_file_read(struct cpu_registers *, uint32_t, uint32_t *);
bool gdb_register_file_write(struct cpu_registers *, uint32_t, uint32_t);
int gdb_getc(void);
void gdb_step(struct cpu_registers *, uint32_t);
void gdb_init(void);
void gdb_monitor_entry(struct cpu_registers *);
void gdb_putc(int);

#endif /* !_SH2_704X_H_ */

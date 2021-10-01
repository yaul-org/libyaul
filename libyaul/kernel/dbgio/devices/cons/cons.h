/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_DBGIO_CONS_H_
#define _KERNEL_DBGIO_CONS_H_

#include <stdbool.h>
#include <stdint.h>

__BEGIN_DECLS

#define CONS_COLS_MIN   40
#define CONS_ROWS_MIN   32

#define CONS_COLS_MAX   64
#define CONS_ROWS_MAX   64

#define CONS_TAB_WIDTH  2

typedef void (*cons_ops_clear)(void);
typedef void (*cons_ops_area_clear)(int16_t, int16_t, int16_t, int16_t);
typedef void (*cons_ops_line_clear)(int16_t);
typedef void (*cons_ops_line_partial_clear)(int16_t, int16_t, int16_t);
typedef void (*cons_ops_write)(int16_t, int16_t, uint8_t);

typedef struct con_ops {
        cons_ops_clear clear;
        cons_ops_area_clear area_clear;
        cons_ops_line_clear line_clear;
        cons_ops_line_partial_clear line_partial_clear;
        cons_ops_write write;
} cons_ops_t;

extern void cons_init(const cons_ops_t *, uint16_t, uint16_t);
extern void cons_buffer(const char *);

__END_DECLS

#endif /* !_KERNEL_DBGIO_CONS_H_ */

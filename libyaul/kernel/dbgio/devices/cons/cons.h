/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_DBGIO_CONS_H_
#define _KERNEL_DBGIO_CONS_H_

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdint.h>

__BEGIN_DECLS

#define CONS_COLS_MIN   40
#define CONS_ROWS_MIN   28

#define CONS_COLS_MAX   64
#define CONS_ROWS_MAX   64

#define CONS_TAB_WIDTH  2

typedef void (*cons_ops_clear_t)(void);
typedef void (*cons_ops_area_clear_t)(int16_t col_start, int16_t col_end, int16_t row_start, int16_t row_end);
typedef void (*cons_ops_line_clear_t)(int16_t row);
typedef void (*cons_ops_line_partial_clear_t)(int16_t col_start, int16_t col_end, int16_t row);
typedef void (*cons_ops_write_t)(int16_t col, int16_t row, uint8_t ch);

typedef struct con_ops {
        cons_ops_clear_t clear;
        cons_ops_area_clear_t area_clear;
        cons_ops_line_clear_t line_clear;
        cons_ops_line_partial_clear_t line_partial_clear;
        cons_ops_write_t write;
} cons_ops_t;

extern void cons_init(const cons_ops_t *ops, uint16_t cols, uint16_t rows);
extern void cons_resize(uint16_t cols, uint16_t rows);
extern void cons_buffer(const char *buffer);

__END_DECLS

#endif /* !_KERNEL_DBGIO_CONS_H_ */

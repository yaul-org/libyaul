/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CONS_H_
#define _CONS_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CONS_COLS_MIN           40
#define CONS_ROWS_MIN           28

#define CONS_COLS_MAX           88
#define CONS_ROWS_MAX           60

#define CONS_TAB_WIDTH          2

#define CONS_DRIVER_VDP1                0
#define CONS_DRIVER_VDP2                1
#define CONS_DRIVER_USB_CARTRIDGE       2

extern void cons_init(uint8_t, uint16_t, uint16_t);
extern void cons_buffer(const char *);
extern void cons_flush(void);
extern void cons_write(const char *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CONS_H_ */

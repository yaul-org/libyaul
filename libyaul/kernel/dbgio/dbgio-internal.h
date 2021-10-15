/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_DBGIO_INTERNAL_H_
#define _KERNEL_DBGIO_INTERNAL_H_

#include <stdbool.h>
#include <stdint.h>

#include <dbgio.h>

__BEGIN_DECLS

typedef void (*font_load_callback_t)(void);

typedef void (*dev_ops_init_t)(const void *);
typedef void (*dev_ops_deinit_t)(void);
typedef void (*dev_ops_font_load_t)(font_load_callback_t);
typedef void (*dev_ops_puts_t)(const char *);
typedef void (*dev_ops_flush_t)(void);

struct dbgio_dev_ops {
        dbgio_dev_t dev;
        const void *default_params;
        dev_ops_init_t init;
        dev_ops_deinit_t deinit;
        dev_ops_font_load_t font_load;
        dev_ops_puts_t puts;
        dev_ops_flush_t flush;
};

extern const struct dbgio_dev_ops _internal_dev_ops_null;
extern const struct dbgio_dev_ops _internal_dev_ops_vdp1;
extern const struct dbgio_dev_ops _internal_dev_ops_vdp2;
extern const struct dbgio_dev_ops _internal_dev_ops_vdp2_async;
extern const struct dbgio_dev_ops _internal_dev_ops_usb_cart;

extern void _internal_dbgio_init(void);

__END_DECLS

#endif /* !_KERNEL_DBGIO_INTERNAL_H_ */

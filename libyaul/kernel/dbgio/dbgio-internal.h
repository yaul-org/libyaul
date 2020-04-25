/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DBGIO_INTERNAL_H_
#define _DBGIO_INTERNAL_H_

#include <stdbool.h>
#include <stdint.h>

__BEGIN_DECLS

typedef void (*dev_ops_init)(const void *);
typedef void (*dev_ops_deinit)(void);
typedef void (*dev_ops_buffer)(const char *);
typedef void (*dev_ops_flush)(void);

struct dbgio_dev_ops {
        uint8_t dev;
        const void *default_params;
        dev_ops_init init;
        dev_ops_deinit deinit;
        dev_ops_buffer buffer;
        dev_ops_flush flush;
};

extern const struct dbgio_dev_ops _internal_dev_ops_null;
extern const struct dbgio_dev_ops _internal_dev_ops_vdp1;
extern const struct dbgio_dev_ops _internal_dev_ops_vdp2_simple;
extern const struct dbgio_dev_ops _internal_dev_ops_vdp2_async;
extern const struct dbgio_dev_ops _internal_dev_ops_usb_cart;

extern void _internal_dbgio_init(void);

__END_DECLS

#endif /* !_DBGIO_INTERNAL_H_ */

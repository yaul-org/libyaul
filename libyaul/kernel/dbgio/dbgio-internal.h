/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DBGIO_INTERNAL_H_
#define _DBGIO_INTERNAL_H_

#include <stdbool.h>
#include <stdint.h>

__BEGIN_DECLS

typedef struct {
        uint8_t dev;
        const void *default_params;
        void (*init)(const void *);
        void (*deinit)(void);
        void (*buffer)(const char *);
        void (*flush)(void);
} dbgio_dev_ops_t;

extern const dbgio_dev_ops_t _internal_dev_ops_null;
extern const dbgio_dev_ops_t _internal_dev_ops_vdp1;
extern const dbgio_dev_ops_t _internal_dev_ops_vdp2_simple;
extern const dbgio_dev_ops_t _internal_dev_ops_vdp2_async;
extern const dbgio_dev_ops_t _internal_dev_ops_usb_cart;

__END_DECLS

#endif /* !_DBGIO_INTERNAL_H_ */

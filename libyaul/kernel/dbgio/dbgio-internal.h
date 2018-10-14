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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
        uint8_t dev;
        const void *default_params;
        void (*init)(const void *);
        void (*buffer)(const char *);
        void (*flush)(void);
} dbgio_dev_ops_t;

extern const dbgio_dev_ops_t _internal_dev_ops_vdp2;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_DBGIO_INTERNAL_H_ */

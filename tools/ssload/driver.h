/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef DRIVER_H_
#define DRIVER_H_

#include <inttypes.h>

struct device_driver {
        const char *name;

        int (*init)(void);
        int (*shutdown)(void);
        const char *(*error_stringify)(void);

        int (*download_buffer)(void *, uint32_t, uint32_t);
        int (*download_file)(const char *, uint32_t, uint32_t);
        int (*upload_buffer)(void *, uint32_t, uint32_t);
        int (*upload_file)(const char *, uint32_t);
        int (*execute_buffer)(void *, uint32_t, uint32_t);
        int (*execute_file)(const char *, uint32_t);
};

#endif /* !DRIVER_H_ */

/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

        int (*read)(void *buffer, uint32_t len);
        int (*send)(void *buffer, uint32_t len);
        int (*download_buffer)(void *buffer, uint32_t base_address, uint32_t len);
        int (*download_file)(const char *output_file, uint32_t base_address, uint32_t len);
        int (*upload_buffer)(void *buffer, uint32_t base_address, uint32_t len);
        int (*upload_file)(const char *input_file, uint32_t base_address);
        int (*execute_buffer)(void *buffer, uint32_t base_address, uint32_t len);
        int (*execute_file)(const char *input_file, uint32_t base_address);
};

#endif /* !DRIVER_H_ */

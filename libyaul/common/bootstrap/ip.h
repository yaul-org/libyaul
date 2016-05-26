/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _IP_H_
#define _IP_H_

#include <inttypes.h>

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct ip_header_data {
        uint8_t hardware_id[16];
        uint8_t manufacture_id[16];
        uint8_t product_id[10];
        uint8_t version[6];
        uint8_t release_date[8];
        uint8_t device_info[8];
        uint8_t area_symbols[10];
        uint8_t bup_info[6];
        uint8_t peripheral_info[16];
        uint8_t title[112];

        /* Reserved area #2 */
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;

        uint32_t ip_length;
        uint32_t master_stack;

        /* Reserved area #3 */
        unsigned int :32;

        uint32_t slave_stack;
        uint32_t boot_address;
        uint32_t boot_size;

        /* Reserved area #3 */
        unsigned int :32;
        unsigned int :32;
} __packed;

struct ip_header_boot {
        uint8_t hardware_id[16];
        uint8_t manufacture_id[16];
        uint8_t product_id[10];
        uint8_t version[6];
        uint8_t release_date[8];
        uint8_t device_info[8];
        uint8_t area_symbols[10];
        uint8_t bup_info[6];

        /* Reserved area #1 */
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;

        uint8_t title[112];

        /* Reserved area #2 */
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;

        /* Reserved area #3 */
        unsigned int :32;

        uint8_t checksum[4];

        /* Reserved area #4 */
        unsigned int :32;
        unsigned int :32;

        /* Reserved area #5 */
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
        unsigned int :32;
} __packed;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_IP_H_ */

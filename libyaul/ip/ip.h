/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_IP_H_
#define _YAUL_IP_H_

#include <sys/cdefs.h>

#include <stdint.h>

/* Hardware IDs */
#define IP_HEADER_DATA_SIGNATURE "SEGASATURN DATA "
#define IP_HEADER_BOOT_SIGNATURE "SEGA SEGASATURN "

/* Device info */
#define IP_HEADER_DEVICE_INFO_ROM       "R"
#define IP_HEADER_DEVICE_INFO_SRAM      "S"
#define IP_HEADER_DEVICE_INFO_DRAM      "D"
#define IP_HEADER_DEVICE_INFO_FRAM      "F"

/* Area codes */
#define IP_HEADER_AREA_CODE_JAPAN               "J"
/* Taiwan, Philippines, Korea */
#define IP_HEADER_AREA_CODE_ASIA_NTSC           "T"
/* USA, Canada, Central and South American NTSC (Brasil) */
#define IP_HEADER_AREA_CODE_AMERICA_NTSC        "U"
/* European PAL, Southeast Asian PAL, Central and South American PAL */
#define IP_HEADER_AREA_CODE_EUROPE_PAL          "E"

/* Back-up RAM info */
#define IP_HEADER_BUP_INFO_BRAM_ODD_BYTES       "O"
#define IP_HEADER_BUP_INFO_BRAM_EVEN_BYTES      "E"
#define IP_HEADER_BUP_INFO_BRAM_WORDS           "W"

typedef struct ip_header_data {
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
} __packed ip_header_data_t;

typedef struct ip_header_boot {
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
} __packed ip_header_boot_t;

#endif /* !_YAUL_IP_H_ */

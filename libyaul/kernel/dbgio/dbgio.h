/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DBGIO_H_
#define _DBGIO_H_

#include <stdbool.h>
#include <stdint.h>

#include <vdp2/vram.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DBGIO_DEV_NULL          0
#define DBGIO_DEV_VDP1          1
#define DBGIO_DEV_VDP2          2
#define DBGIO_DEV_USB_CART      3

#define DBGIO_DEV_COUNT         4

typedef struct {
        const uint8_t *font_cpd;
        const uint16_t *font_pal;
        uint8_t font_fg;
        uint8_t font_bg;

        uint8_t scrn;

        uint8_t cpd_bank;
        uint32_t cpd_offset;

        uint8_t pnd_bank;
        uint8_t pnd_offset;

        struct vram_cycp_bank cpd_cycp;
        struct vram_cycp_bank pnd_cycp;

        uint8_t cram_index;
} dbgio_vdp2_t;

typedef struct {
        uint16_t buffer_size;
} dbgio_usb_cart_t;

extern void dbgio_init(void);
extern void dbgio_dev_init(uint8_t, const void *);
extern void dbgio_dev_default_init(uint8_t);
extern void dbgio_dev_set(uint8_t);
extern void dbgio_buffer(const char *);
extern void dbgio_flush(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_DBGIO_H_ */

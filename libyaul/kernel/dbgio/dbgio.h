/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_DBGIO_H_
#define _YAUL_KERNEL_DBGIO_H_

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdint.h>

#include <vdp2/scrn.h>
#include <vdp2/vram.h>

__BEGIN_DECLS

typedef enum dbgio_dev {
        DBGIO_DEV_NULL        = 0,
        DBGIO_DEV_VDP1        = 1,
        DBGIO_DEV_VDP2_SIMPLE = 2,
        DBGIO_DEV_VDP2_ASYNC  = 3,
        DBGIO_DEV_USB_CART    = 4
} dbgio_dev_t;

#define DBGIO_DEV_COUNT 5

typedef struct dbgio_vdp2 {
        const uint8_t *font_cpd;
        const uint16_t *font_pal;
        uint8_t font_fg;
        uint8_t font_bg;

        vdp2_scrn_t scroll_screen;

        vdp2_vram_bank_t cpd_bank;
        vdp2_vram_t cpd_offset;

        vdp2_vram_bank_t pnd_bank;
        uint8_t map_index;

        vdp2_vram_cycp_bank_t cpd_cycp;
        vdp2_vram_cycp_bank_t pnd_cycp;

        uint8_t cram_index;
} dbgio_vdp2_t;

typedef struct dbgio_usb_cart {
        uint16_t buffer_size;
} dbgio_usb_cart_t;

extern void dbgio_dev_init(dbgio_dev_t, const void *);
extern void dbgio_dev_default_init(dbgio_dev_t);
extern void dbgio_dev_deinit(void);

extern void dbgio_dev_font_load(void);
extern void dbgio_dev_font_load_wait(void);

extern void dbgio_puts(const char *);
extern void dbgio_printf(const char *, ...) __printflike(1, 2);
extern void dbgio_flush(void);

__END_DECLS

#endif /* !_YAUL_KERNEL_DBGIO_H_ */

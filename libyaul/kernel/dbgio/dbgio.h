/*
 * Copyright (c) Israel Jacquez
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
    DBGIO_DEV_NULL,
    DBGIO_DEV_VDP1,
    DBGIO_DEV_VDP2,
    DBGIO_DEV_VDP2_ASYNC,
    DBGIO_DEV_USB_CART,
    DBGIO_DEV_MEDNAFEN_DEBUG
} dbgio_dev_t;

#define DBGIO_DEV_COUNT 6

typedef struct dbgio_font {
    const uint8_t *cg;
    size_t cg_size;
    const uint16_t *pal;
    size_t pal_size;
    uint8_t fg_pal;
    uint8_t bg_pal;
} dbgio_font_t;

typedef struct dbgio_vdp2_charmap {
    uint16_t pnd;
} dbgio_vdp2_charmap_t;

typedef struct dbgio_vdp2 {
    const dbgio_font_t *font;
    const dbgio_vdp2_charmap_t *font_charmap;

    const vdp2_scrn_cell_format_t *cell_format;
    const vdp2_scrn_normal_map_t *normal_map;
} dbgio_vdp2_t;

extern void dbgio_init(void);

extern void dbgio_dev_init(dbgio_dev_t dev, const void *params);
extern void dbgio_dev_default_init(dbgio_dev_t dev);
extern void dbgio_dev_deinit(void);
extern dbgio_dev_t dbgio_dev_selected_get(void);

extern void dbgio_dev_font_load(void);

extern void dbgio_display_set(bool display);

extern void dbgio_puts(const char *buffer);
extern void dbgio_printf(const char *format, ...) __printflike(1, 2);
extern void dbgio_flush(void);

__END_DECLS

#endif /* !_YAUL_KERNEL_DBGIO_H_ */

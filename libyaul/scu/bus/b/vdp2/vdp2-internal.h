/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_INTERNAL_H_
#define _VDP2_INTERNAL_H_

#include <scu-internal.h>
#include <vdp2/map.h>

struct vdp2_regs {
        uint16_t bgon;          /* Register offset: 0x180020 */
        uint16_t mzctl;         /* Register offset: 0x180022 */
        uint16_t chctla;        /* Register offset: 0x180028 */
        uint16_t chctlb;        /* Register offset: 0x18002A */
        uint16_t bmpna;         /* Register offset: 0x18002C */
        uint16_t bmpnb;         /* Register offset: 0x18002E */
        uint16_t plsz;          /* Register offset: 0x18003A */
        uint16_t mpofn;         /* Register offset: 0x18003C */
        uint16_t mpofr;         /* Register offset: 0x18003E */
        uint16_t scrctl;        /* Register offset: 0x18009A */
        uint16_t rpmd;          /* Register offset: 0x1800B0 */
        uint16_t prina;         /* Register offset: 0x1800F8 */
        uint16_t prinb;         /* Register offset: 0x1800FA */
        uint16_t prir;          /* Register offset: 0x1800FC */
};

extern struct vdp2_regs vdp2_regs;
extern struct vram_ctl vram_ctl;

extern void vdp2_scrn_character_color_count_set(uint8_t, uint32_t);

#endif /* !_VDP2_INTERNAL_H_ */

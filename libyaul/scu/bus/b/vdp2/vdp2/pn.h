/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _PN_H_
#define _PN_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Configuration #0
 * 1-word
 * 1x1 character size
 * Auxiliary mode 0 */
#define VDP2_PN_CONFIG_0_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        (((x) >> 5) & 0x03FF)

/* Configuration #1
 * 1-word
 * 1x1 character size
 * Auxiliary mode 1 */
#define VDP2_PN_CONFIG_1_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        (((x) >> 5) & 0x0FFF)

/* Configuration #2
 * 1-word
 * 2x2 character size
 * Auxiliary mode 0 */
#define VDP2_PN_CONFIG_2_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        ((((x) >> 5) & 0x0FFC) >> 2)

/* Configuration #3
 * 1-word
 * 2x2 character size
 * Auxiliary mode 1 */
#define VDP2_PN_CONFIG_3_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        ((((x) >> 5) & 0x3FFC) >> 2)

/* Configuration #4
 * 2-words
 * 1x1 character size */
#define VDP2_PN_CONFIG_4_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        (((x) >> 5) & 0x7FFF)

/* Configuration #5
 * 2-words
 * 2x2 character size */
#define VDP2_PN_CONFIG_5_CHARACTER_NUMBER(x) VDP2_PN_CONFIG_4_CHARACTER_NUMBER(x)

/* Configuration #1
 * 1-word
 * 16 colors */
#define VDP2_PN_CONFIG_1_PALETTE_NUMBER(x)                                     \
        ((((x) >> 5) & 0x0F) << 12)

/* Configuration #2
 * 1-word
 * 256 and 2,048 colors */
#define VDP2_PN_CONFIG_2_PALETTE_NUMBER(x)                                     \
        ((((x) >> 5) & 0x70) << 12)

/* Configuration #3
 * 2-words
 * 16, 256, and 2,048 colors */
#define VDP2_PN_CONFIG_3_PALETTE_NUMBER(x)                                     \
        ((((x) >> 5) & 0x7F) << 16)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_PN_H_ */

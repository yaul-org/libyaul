/*
 * Copyright (c) 2012 Israel Jacquez
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
 * Word size: 1
 * Character size: 1x1
 * Character number supplement mode: 0 */
#define VDP2_PN_CONFIG_0_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        (((x) >> 5) & 0x03FF)

/* Configuration #1
 * Word size: 1
 * Character size: 1x1
 * Character number supplement mode: 1 */
#define VDP2_PN_CONFIG_1_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        (((x) >> 5) & 0x0FFF)

/* Configuration #2
 * Word size: 1
 * Character size: 2x2
 * Character number supplement mode: 0 */
#define VDP2_PN_CONFIG_2_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        ((((x) >> 5) & 0x0FFC) >> 2)

/* Configuration #3
 * Word size: 1
 * Character size: 2x2
 * Character number supplement mode: 1 */
#define VDP2_PN_CONFIG_3_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        ((((x) >> 5) & 0x3FFC) >> 2)

/* Configuration #4
 * Word size: 2
 * Character size: 1x1 */
#define VDP2_PN_CONFIG_4_CHARACTER_NUMBER(x)                                   \
        /* First convert the address into a character number; each cell
         * is on a 20-byte boundary */                                         \
        (((x) >> 5) & 0x7FFF)

/* Configuration #5
 * Word size: 2
 * Character size: 2x2 */
#define VDP2_PN_CONFIG_5_CHARACTER_NUMBER(x) VDP2_PN_CONFIG_4_CHARACTER_NUMBER(x)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_PN_H_ */

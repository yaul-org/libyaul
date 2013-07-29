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

/* Given a VRAM address X, calculate the character number in the pattern
 * name table for 1-word. */
#define PN_CHARACTER_NO(x)      (((x) >> 5) & 0x0FFF)

/* Given a VRAM address X, calculate the character number in the pattern
 * name table for 2-word. */
#define PN_CHARACTER_NO_2WORD(x) (((x) >> 5) & 0x7FFF)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_PN_H_ */

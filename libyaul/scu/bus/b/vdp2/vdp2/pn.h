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
 * name table. */
#define PN_CHARACTER_NO(x)      (((x) >> 5) & 0x0FFF)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_PN_H_ */

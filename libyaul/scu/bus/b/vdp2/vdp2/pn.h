/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _PN_H_
#define _PN_H_

/* Given a VRAM address X, calculate the character number in the pattern
 * name table. */
#define PN_CHARACTER_NO(x)      (((x) >> 5) & 0x0FFF)

#endif /* !_PN_H_ */

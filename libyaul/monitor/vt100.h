/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _VT100_H_
#define _VT100_H_

struct cha {
        int fg;
        int bg;
        bool is_trans;
};

#define TERM_TAB_WIDTH 4

typedef void (*mwrite)(int, struct cha *, int);

extern int vt100_write(const char *, mwrite);

#endif /* !_SMPC_H_ */

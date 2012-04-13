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

typedef void (*write_hdl)(int, struct cha *);

extern int vt100_write(write_hdl, const char *);

#endif /* !_SMPC_H_ */

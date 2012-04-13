/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _VT100_H_
#define _VT100_H_

#define BACKGROUND      0
#define FOREGROUND      7

struct cha {
        uint8_t fg;
        uint8_t bg;
        bool is_trans; /* Is it transparent */
};

typedef void (*write_hdl)(int, struct cha *);

extern int vt100_write(write_hdl, const char *);

#endif /* !_SMPC_H_ */

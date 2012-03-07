/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _CMDT_H_
#define _CMDT_H_

struct cmdt_cmd {
        unsigned short  ctrl;
        unsigned short  link;
        unsigned short  pmod;
        unsigned short  colr;
        unsigned short  srca;
        unsigned short  size;
        signed short    xa;
        signed short    ya;
        signed short    xb;
        signed short    yb;
        signed short    xc;
        signed short    yc;
        signed short    xd;
        signed short    yd;
        unsigned short  grda;
        unsigned short  reserved;
};

struct cmdt_grd {
        unsigned short  entry[4];
};

#endif

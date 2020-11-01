/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "sega3d.h"

static POINT point_PLANE1[] = {
        POStoFIXED(-10.0, -10.0, 0.0),
        POStoFIXED( 10.0, -10.0, 0.0),
        POStoFIXED( 10.0,  10.0, 0.0),
        POStoFIXED(-10.0,  10.0, 0.0),
};

static POLYGON polygon_PLANE1[] = {
        NORMAL(0.0, 1.0, 0.0), VERTICES(0, 1, 2, 3)
};

static ATTR attribute_PLANE1[] = {
        ATTRIBUTE(Dual_Plane, SORT_CEN, No_Texture, C_RGB(31, 31, 31), No_Gouraud, MESHoff, sprPolygon, No_Option)
};

PDATA PD_PLANE1 = {
        point_PLANE1,
        sizeof(point_PLANE1) / sizeof(POINT),
        polygon_PLANE1,
        sizeof(polygon_PLANE1) / sizeof(POLYGON),
        attribute_PLANE1
};

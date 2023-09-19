/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_STATE_H_
#define _MIC3D_STATE_H_

#include <gamemath/fix16.h>

#include "mic3d.h"

typedef struct render render_t;
typedef struct sort sort_t;
typedef struct tlist tlist_t;
typedef struct light light_t;
typedef struct gst gst_t;
typedef struct mstack mstack_t;

typedef struct perf perf_t;

typedef struct {
    mic3d_workarea_t *workarea;
    render_t *render;
    sort_t *sort;
    tlist_t *tlist;
    mstack_t *mstack;
    light_t *light;
    gst_t *gst;
    perf_t *perf;
} state_t;

extern state_t __state;

#endif /* _MIC3D_STATE_H_ */

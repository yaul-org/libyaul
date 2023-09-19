/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include "internal.h"

static render_t _render;
static sort_t _sort;
static tlist_t _tlist;
static light_t _light;
static gst_t _gst;
static perf_t _perf;

void
mic3d_init(mic3d_workarea_t *workarea)
{
    assert(workarea != NULL);

    assert(workarea->depth_values != NULL);
    assert(workarea->z_values != NULL);
    assert(workarea->screen_points != NULL);
    assert(workarea->sort_lists != NULL);
    assert(workarea->sort_singles != NULL);
    assert(workarea->cmdts != NULL);
    assert(workarea->render_matrices != NULL);
    assert(workarea->light_matrices != NULL);
    assert(workarea->colors != NULL);
    assert(workarea->work != NULL);

    assert((((uintptr_t)workarea->depth_values) & (WORKAREA_DEPTH_VALUES_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->z_values) & (WORKAREA_Z_VALUES_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->screen_points) & (WORKAREA_SCREEN_POINTS_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->sort_lists) & (WORKAREA_SORT_LISTS_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->sort_singles) & (WORKAREA_SORT_SINGLES_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->cmdts) & (WORKAREA_CMDTS_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->render_matrices) & (WORKAREA_RENDER_MATRICES_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->light_matrices) & (WORKAREA_LIGHT_MATRICES_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->colors) & (WORKAREA_COLORS_ALIGNMENT - 1)) == 0);
    assert((((uintptr_t)workarea->work) & (WORKAREA_WORK_ALIGNMENT - 1)) == 0);

    __state.workarea = workarea;
    __state.render = &_render;
    __state.sort = &_sort;
    __state.tlist = &_tlist;
    __state.light = &_light;
    __state.gst = &_gst;
    __state.perf = &_perf;

    __render_init();
    __sort_init();
    __tlist_init();
    __light_init();
    __perf_init();
    __gst_init();
}

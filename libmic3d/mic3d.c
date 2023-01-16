/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "internal.h"

static render_t _render;
static sort_t _sort;
static tlist_t _tlist;
static mat_stack_t _mat_stack;
static light_t _light;
static gst_t _gst;
static perf_t _perf;

void
mic3d_init(void)
{
        __state.render = &_render;
        __state.sort = &_sort;
        __state.tlist = &_tlist;
        __state.mat_stack = &_mat_stack;
        __state.light = &_light;
        __state.gst = &_gst;
        __state.perf = &_perf;

        __render_init();
        __sort_init();
        __tlist_init();
        __matrix_init();
        __light_init();
        __perf_init();
        __gst_init();
}

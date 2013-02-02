/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <bus/cpu/cpu.h>

#include "irq-mux.h"

static irq_mux_handle_t *irq_mux_handle_alloc(void);
static void irq_mux_handle_free(irq_mux_handle_t *);

void
irq_mux_init(irq_mux_t *irq_mux)
{
        TAILQ_INIT(&irq_mux->im_tq);

        irq_mux->im_total = 0;
}

void
irq_mux_handle(irq_mux_t *irq_mux)
{
        irq_mux_handle_t *hdl_np;

        if (TAILQ_EMPTY(&irq_mux->im_tq))
                return;

        TAILQ_FOREACH(hdl_np, &irq_mux->im_tq, handles) {
                hdl_np->imh_hdl(hdl_np);
        }
}

void
irq_mux_handle_add(irq_mux_t *irq_mux, void (*hdl)(irq_mux_handle_t *), void *user_data)
{

        irq_mux_handle_t *n_hdl;

        /* Disable interrupts */
        cpu_intc_disable();

        n_hdl = irq_mux_handle_alloc();
        assert(n_hdl != NULL);
        n_hdl->imh_hdl = hdl;
        n_hdl->imh_user_ptr = user_data;
        n_hdl->imh = irq_mux;

        irq_mux->im_total++;
        TAILQ_INSERT_TAIL(&irq_mux->im_tq, n_hdl, handles);

        /* Enable interrupts */
        cpu_intc_enable();
}

void
irq_mux_handle_remove(irq_mux_t *irq_mux, void (*hdl)(irq_mux_handle_t *))
{
        irq_mux_handle_t *hdl_np;

        if (TAILQ_EMPTY(&irq_mux->im_tq))
                return;

        TAILQ_FOREACH(hdl_np, &irq_mux->im_tq, handles) {
                if (hdl_np->imh_hdl == hdl) {
                        TAILQ_REMOVE(&irq_mux->im_tq, hdl_np, handles);
                        irq_mux_handle_free(hdl_np);
                        return;
                }
        }
}

static irq_mux_handle_t *
irq_mux_handle_alloc(void)
{
        irq_mux_handle_t *n_hdl;

        if ((n_hdl = (irq_mux_handle_t *)malloc(sizeof(irq_mux_handle_t))) == NULL)
                return NULL;

        return n_hdl;
}

static void
irq_mux_handle_free(irq_mux_handle_t *hdl)
{

        free(hdl);
}

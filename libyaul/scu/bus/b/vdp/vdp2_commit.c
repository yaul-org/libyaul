/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/vram.h>

#include <sys/dma-queue.h>

#include "vdp-internal.h"

static void _commit_handler(void *);
static void _default_handler(void *);

static bool _blocking;
static void (*_handler)(void *);
static void *_work;

void
vdp2_commit(void)
{
        struct dma_reg_buffer *reg_buffer;
        reg_buffer = &_state_vdp2()->commit.reg_buffer;

        int8_t ret;
        ret = dma_queue_enqueue(reg_buffer, DMA_QUEUE_TAG_VBLANK_IN, _commit_handler, _work);
        assert(ret == 0);
}

void
vdp2_commit_wait(void)
{
        /* Don't wait until the entire DMA queue requests tagged as
         * immediate is clear */
        while (_blocking) {
        }
}

void
vdp2_commit_handler_set(void (*handler)(void *), void *work)
{
        _handler = (handler != NULL) ? handler : _default_handler;
        _work = work;
}

static void
_commit_handler(void *work)
{
        /* Mark that our DMA queue request has been fulfilled */
        _blocking = false;

        _handler(work);
}

static void
_default_handler(void *work __unused)
{
}

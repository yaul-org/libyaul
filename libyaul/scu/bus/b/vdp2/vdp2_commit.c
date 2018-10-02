/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/vram.h>

#include <sys/dma-queue.h>

#include "vdp2-internal.h"

static void _default_handler(void *);

static bool _blocking;

void
vdp2_commit(void)
{
        void *commit_dma_buffer;
        commit_dma_buffer = &_internal_state_vdp2.commit_dma_buffer[0];

        int8_t ret;
        ret = dma_queue_enqueue(commit_dma_buffer, DMA_QUEUE_TAG_VBLANK_IN);
        assert(ret == 0);
}

void
vdp2_commit_block(void)
{
        void *commit_dma_buffer;
        commit_dma_buffer = &_internal_state_vdp2.commit_dma_buffer[0];

        int8_t ret;
        ret = dma_queue_enqueue(commit_dma_buffer, DMA_QUEUE_TAG_IMMEDIATE);
        assert(ret == 0);

        _blocking = true;

        ret = dma_queue_flush(DMA_QUEUE_TAG_IMMEDIATE);
        assert(ret == 0);

        /* Don't wait until the entire DMA queue requests tagged as
         * immediate is clear */
        while (_blocking) {
        }
}

void
vdp2_commit_handler_set(void (*handler)(void *), void *work)
{
        _internal_state_vdp2.commit_dma_handler =
            (handler != NULL) ? handler : _default_handler;
        _internal_state_vdp2.commit_dma_work = work;
}

void
_internal_vdp2_commit_handler(void *work __unused)
{
        /* Mark that our DMA queue request has been fulfilled */
        _blocking = false;

        _internal_state_vdp2.commit_dma_handler(_internal_state_vdp2.commit_dma_work);
}

static void
_default_handler(void *work __unused)
{
}

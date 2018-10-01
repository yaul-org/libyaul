/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include <sys/dma-queue.h>

#include <scu-internal.h>

/* Default SCU DMA level */
#define DMA_QUEUE_SCU_DMA_LEVEL         0
#define DMA_QUEUE_SCU_DMA_INTERRUPT     IC_INTERRUPT_LEVEL_0_DMA_END
#define DMA_QUEUE_SCU_DMA_MASK_DISABLE  IC_MASK_LEVEL_0_DMA_END
#define DMA_QUEUE_SCU_DMA_MASK_ENABLE   (~DMA_QUEUE_SCU_DMA_MASK_DISABLE & IC_MASK_ALL)
#define DMA_QUEUE_SCU_DMA_IST           IC_IST_LEVEL_0_DMA_END

/* Maximum count of requests in ring buffer to take in */
#define DMA_QUEUE_REQUESTS_MAX_COUNT    32

struct dma_queue {
        struct dma_queue_request {
                uint8_t tag;
                struct state_scu_dma_level state;

                void (*handler)(void *);
                void *work;
        } requests[DMA_QUEUE_REQUESTS_MAX_COUNT];

        uint16_t head;
        uint16_t tail;
        uint16_t count;
};

static void _update_dma_request_pointers(const struct dma_queue_request *);
static void _start_dma_request(const struct dma_queue_request *);
static void _copy_dma_request_state(struct state_scu_dma_level *, const void *);

static void _default_handler(void *);

static void _dma_handler(void);

static struct dma_queue _dma_queues[DMA_QUEUE_TAG_COUNT];
static const struct dma_queue_request *_current_request;
static const struct dma_queue_request *_last_request;

void
dma_queue_init(void)
{
        uint32_t tag;
        for (tag = 0; tag < DMA_QUEUE_TAG_COUNT; tag++) {
                _dma_queues[tag].head = 0;
                _dma_queues[tag].tail = 0;
                _dma_queues[tag].count = 0;
        }

        _current_request = NULL;
        _last_request = NULL;

        scu_dma_level_wait(DMA_QUEUE_SCU_DMA_LEVEL);
        scu_dma_level_stop(DMA_QUEUE_SCU_DMA_LEVEL);

        scu_ic_ihr_set(DMA_QUEUE_SCU_DMA_INTERRUPT, _dma_handler);
}

int8_t
dma_queue_enqueue(const void *buffer, uint8_t tag)
{
        assert(buffer != NULL);

        assert(tag < DMA_QUEUE_TAG_COUNT);

        int8_t error;
        error = 0;

        scu_ic_mask_chg(IC_MASK_ALL, DMA_QUEUE_SCU_DMA_MASK_DISABLE);

        struct dma_queue *dma_queue;
        dma_queue = &_dma_queues[tag];

        assert((dma_queue->head >= 0) && (dma_queue->head <= DMA_QUEUE_REQUESTS_MAX_COUNT));
        assert((dma_queue->tail >= 0) && (dma_queue->tail <= DMA_QUEUE_REQUESTS_MAX_COUNT));
        assert((dma_queue->count >= 0) && (dma_queue->count <= DMA_QUEUE_REQUESTS_MAX_COUNT));

        if (dma_queue->count >= DMA_QUEUE_REQUESTS_MAX_COUNT) {
                error = -1;
                goto exit;
        }

        /* When the tail pointer wraps around the ring buffer */
        if ((dma_queue->count > 0) && (dma_queue->tail == dma_queue->head)) {
                error = -1;
                goto exit;
        }

        /* No processing of the ring buffer has occured (head pointer is
         * at the beginning of the ring buffer) */
        struct dma_queue_request *request;
        request = &dma_queue->requests[dma_queue->tail];

        request->tag = tag;

        _copy_dma_request_state(&request->state, buffer);

        request->tag = tag;
        request->handler = (request->state.ihr != NULL)
            ? request->state.ihr
            : _default_handler;

        dma_queue->tail++;
        dma_queue->count++;

exit:
        dma_queue->tail &= DMA_QUEUE_REQUESTS_MAX_COUNT - 1;

        scu_ic_mask_chg(DMA_QUEUE_SCU_DMA_MASK_ENABLE, IC_MASK_NONE);

        return error;
}

int8_t
dma_queue_flush(uint8_t tag)
{
        assert(tag < DMA_QUEUE_TAG_COUNT);

        bool dma_busy;
        dma_busy = ((scu_dma_level_busy(DMA_QUEUE_SCU_DMA_LEVEL)) != 0x00);

        if (dma_busy) {
                return -1;
        }

        scu_ic_mask_chg(IC_MASK_ALL, DMA_QUEUE_SCU_DMA_MASK_DISABLE);

        struct dma_queue *dma_queue;
        dma_queue = &_dma_queues[tag];

        struct dma_queue_request *request;
        request = &dma_queue->requests[dma_queue->head];

        uint32_t queue_count;
        queue_count = dma_queue->count;

        if (queue_count > 0) {
                _update_dma_request_pointers(request);
        }

        scu_ic_mask_chg(DMA_QUEUE_SCU_DMA_MASK_ENABLE, IC_MASK_NONE);

        if (queue_count == 0) {
                return 0;
        }

        _start_dma_request(request);

        return 0;
}

uint32_t
dma_queue_count_get(uint8_t tag)
{
        assert(tag < DMA_QUEUE_TAG_COUNT);

        struct dma_queue *dma_queue;
        dma_queue = &_dma_queues[tag];

        return dma_queue->count;
}

uint32_t
dma_queue_capacity_get(void)
{
        return DMA_QUEUE_REQUESTS_MAX_COUNT;
}

static void __attribute__ ((always_inline)) inline
_update_dma_request_pointers(const struct dma_queue_request *request)
{
        assert(request != NULL);

        _last_request = _current_request;
        _current_request = request;
}

static void __attribute__ ((always_inline)) inline
_start_dma_request(const struct dma_queue_request *request)
{
        assert(request != NULL);

        const struct state_scu_dma_level *state;
        state = &request->state;

        scu_dma_level_buffer_set(state);
        scu_dma_level_fast_start(DMA_QUEUE_SCU_DMA_LEVEL);
}

static void __attribute__ ((always_inline)) inline
_copy_dma_request_state(struct state_scu_dma_level *dst_state, const void *buffer)
{
        const struct state_scu_dma_level *src_state;
        src_state = buffer;

        uint32_t *dst;
        dst = &dst_state->buffered_regs.buffer[0];

        const uint32_t *src;
        src = &src_state->buffered_regs.buffer[0];

        *(dst++) = *(src++);
        *(dst++) = *(src++);
        *(dst++) = *(src++);
        *(dst++) = *(src++);
        *(dst++) = *(src++);

        dst_state->level = src_state->level;
        dst_state->ihr = src_state->ihr;
        dst_state->work = src_state->work;
}

static void
_dma_handler(void)
{
        scu_dma_level_stop(DMA_QUEUE_SCU_DMA_LEVEL);

        struct dma_queue *dma_queue;
        dma_queue = &_dma_queues[_current_request->tag];

        void (*handler)(void *);
        handler = _current_request->handler;
        void *work;
        work = _current_request->work;

        dma_queue->head++;
        dma_queue->head &= DMA_QUEUE_REQUESTS_MAX_COUNT - 1;
        dma_queue->count--;

        /* Finished with the current request */
        _current_request = NULL;

        if (dma_queue->count > 0) {
                struct dma_queue_request *next_request;
                next_request = &dma_queue->requests[dma_queue->head];

                _update_dma_request_pointers(next_request);
                _start_dma_request(next_request);
        }

        handler(work);
}

static void
_default_handler(void *work __unused)
{
}

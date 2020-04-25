/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <cpu/registers.h>
#include <cpu/intc.h>
#include <cpu/cache.h>

#include <sys/dma-queue.h>

#include <scu-internal.h>

#include <gdb.h>

/* Default SCU DMA level */
#define DMA_QUEUE_SCU_DMA_LEVEL         0
#define DMA_QUEUE_SCU_DMA_MASK          SCU_IC_MASK_LEVEL_0_DMA_END

/* Maximum count of requests in ring buffer to take in */
#define DMA_QUEUE_REQUESTS_MAX_COUNT    (32)
#define DMA_QUEUE_REQUESTS_MASK         (DMA_QUEUE_REQUESTS_MAX_COUNT - 1)

typedef void (*dma_queue_request_handler)(const dma_queue_transfer_t *);

struct dma_queue_request {
        scu_dma_handle_t handle;

        dma_queue_request_handler handler;
        dma_queue_transfer_t transfer;
} __aligned(8);

struct dma_queue {
        struct dma_queue_request requests[DMA_QUEUE_REQUESTS_MAX_COUNT];

        volatile int8_t head;
        volatile int8_t tail;
} __aligned(16);

static_assert(sizeof(struct dma_queue_request) == 32);
static_assert(sizeof(struct dma_queue) == 1040);

static inline void _queue_init(struct dma_queue *) __always_inline;
static inline uint32_t _queue_size(const struct dma_queue *) __always_inline;
static inline bool _queue_full(const struct dma_queue *) __always_inline;
static inline bool _queue_empty(const struct dma_queue *) __always_inline;
static inline struct dma_queue_request * _queue_top(struct dma_queue *) __always_inline;
static inline struct dma_queue_request * _queue_enqueue(struct dma_queue *) __always_inline;
static inline struct dma_queue_request * _queue_dequeue(struct dma_queue *) __always_inline;

static inline void _dma_queue_request_start(const struct dma_queue_request *) __always_inline;

static void _default_handler(const dma_queue_transfer_t *);

static void _dma_handler(void);
static void _dma_illegal_handler(void);

static struct {
        struct dma_queue dma_queues[DMA_QUEUE_TAG_COUNT];

        volatile uint8_t current_tag;
} _state;

void
_internal_dma_queue_init(void)
{
        scu_dma_level_wait(DMA_QUEUE_SCU_DMA_LEVEL);
        scu_dma_level_stop(DMA_QUEUE_SCU_DMA_LEVEL);

        scu_dma_illegal_set(_dma_illegal_handler);

        scu_dma_level0_end_set(_dma_handler);

        uint32_t tag;
        for (tag = 0; tag < DMA_QUEUE_TAG_COUNT; tag++) {
                struct dma_queue *dma_queue;
                dma_queue = &_state.dma_queues[tag];

                _queue_init(dma_queue);

                for (uint32_t i = 0; i < DMA_QUEUE_REQUESTS_MAX_COUNT; i++) {
                        struct dma_queue_request *request;
                        request = &dma_queue->requests[i];

                        request->handler = _default_handler;

                        request->transfer.status = DMA_QUEUE_STATUS_UNKNOWN;
                        request->transfer.work = NULL;
                }
        }

        _state.current_tag = DMA_QUEUE_TAG_INVALID;
}

int8_t
dma_queue_enqueue(const scu_dma_handle_t *handle, uint8_t tag,
    dma_queue_request_handler handler,
    void *work)
{
        assert(handle != NULL);

        assert(tag < DMA_QUEUE_TAG_COUNT);

        struct dma_queue *dma_queue;
        dma_queue = &_state.dma_queues[tag];

        uint32_t scu_mask;
        scu_mask = scu_ic_mask_get();

        scu_ic_mask_chg(SCU_IC_MASK_ALL, DMA_QUEUE_SCU_DMA_MASK);

        if (_queue_full(dma_queue)) {
                scu_ic_mask_set(scu_mask);

                return -1;
        }

        struct dma_queue_request *request;
        request = _queue_enqueue(dma_queue);
        assert(request != NULL);
        scu_ic_mask_set(scu_mask);

        (void)memcpy(&request->handle, handle, sizeof(scu_dma_handle_t));

        request->handler = (handler != NULL) ? handler : _default_handler;
        request->transfer.status = DMA_QUEUE_STATUS_INCOMPLETE;
        request->transfer.work = work;

        return 0;
}

void
dma_queue_tag_clear(uint8_t tag)
{
        assert(tag < DMA_QUEUE_TAG_COUNT);

        dma_queue_flush_wait();

        uint32_t scu_mask;
        scu_mask = scu_ic_mask_get();

        struct dma_queue *dma_queue;
        dma_queue = &_state.dma_queues[tag];

        scu_ic_mask_chg(SCU_IC_MASK_ALL, DMA_QUEUE_SCU_DMA_MASK);

        while (!_queue_empty(dma_queue)) {
                struct dma_queue_request *request;
                request = _queue_dequeue(dma_queue);

                request->transfer.status = DMA_QUEUE_STATUS_CANCELED;

                request->handler(&request->transfer);
        }

        scu_ic_mask_set(scu_mask);
}

void
dma_queue_clear(void)
{
        uint32_t tag;
        for (tag = 0; tag < DMA_QUEUE_TAG_COUNT; tag++) {
                dma_queue_tag_clear(tag);
        }
}

uint32_t
dma_queue_flush(uint8_t tag)
{
        assert(tag < DMA_QUEUE_TAG_COUNT);

        dma_queue_flush_wait();

        struct dma_queue *dma_queue;
        dma_queue = &_state.dma_queues[tag];

        uint32_t scu_mask;
        scu_mask = scu_ic_mask_get();

        scu_ic_mask_chg(SCU_IC_MASK_ALL, DMA_QUEUE_SCU_DMA_MASK);

        if (_queue_empty(dma_queue)) {
                goto exit;
        }

        _state.current_tag = tag;

        struct dma_queue_request *request;
        request = _queue_dequeue(dma_queue);

        _dma_queue_request_start(request);

        scu_mask &= ~DMA_QUEUE_SCU_DMA_MASK;

exit:
        scu_ic_mask_set(scu_mask);

        return 0;
}

void
dma_queue_flush_wait(void)
{
        while (_state.current_tag != DMA_QUEUE_TAG_INVALID) {
        }
}

uint32_t
dma_queue_count_get(uint8_t tag)
{
        assert(tag < DMA_QUEUE_TAG_COUNT);

        struct dma_queue *dma_queue;
        dma_queue = &_state.dma_queues[tag];

        uint32_t scu_mask;
        scu_mask = scu_ic_mask_get();

        scu_ic_mask_chg(SCU_IC_MASK_ALL, DMA_QUEUE_SCU_DMA_MASK);

        uint32_t size;
        size = _queue_size(dma_queue);

        scu_ic_mask_set(scu_mask);

        return size;
}

uint32_t
dma_queue_capacity_get(void)
{
        return DMA_QUEUE_REQUESTS_MAX_COUNT;
}

static inline void __always_inline
_queue_init(struct dma_queue *dma_queue)
{
        dma_queue->head = -1;
        dma_queue->tail = -1;
}

static inline uint32_t __always_inline
_queue_size(const struct dma_queue *dma_queue)
{
        return (dma_queue->tail - dma_queue->head);
}

static inline bool __always_inline
_queue_full(const struct dma_queue *dma_queue)
{
        return (_queue_size(dma_queue) == DMA_QUEUE_REQUESTS_MAX_COUNT);
}

static inline bool __always_inline
_queue_empty(const struct dma_queue *dma_queue)
{
        return (dma_queue->head == dma_queue->tail);
}

static inline struct dma_queue_request * __always_inline
_queue_top(struct dma_queue *dma_queue)
{
        struct dma_queue_request *request;
        request = &dma_queue->requests[dma_queue->head & DMA_QUEUE_REQUESTS_MASK];

        return request;
}

static inline struct dma_queue_request * __always_inline
_queue_enqueue(struct dma_queue *dma_queue)
{
        assert (!_queue_full(dma_queue));

        const int8_t next_tail = (dma_queue->tail + 1) & DMA_QUEUE_REQUESTS_MASK;

        struct dma_queue_request *request;
        request = &dma_queue->requests[next_tail];

        dma_queue->tail++;

        return request;
}

static inline struct dma_queue_request * __always_inline
_queue_dequeue(struct dma_queue *dma_queue)
{
        assert(!_queue_empty(dma_queue));

        const int8_t next_head = (dma_queue->head + 1) & DMA_QUEUE_REQUESTS_MASK;

        struct dma_queue_request *request;
        request = &dma_queue->requests[next_head & DMA_QUEUE_REQUESTS_MASK];

        dma_queue->head++;

        return request;
}

static inline void __always_inline
_dma_queue_request_start(const struct dma_queue_request *request)
{
        scu_dma_config_set(DMA_QUEUE_SCU_DMA_LEVEL, SCU_DMA_START_FACTOR_ENABLE,
            &request->handle, _dma_handler);

        /* We need to purge the cache before starting the transfer */
        cpu_cache_purge();

        scu_dma_level_fast_start(DMA_QUEUE_SCU_DMA_LEVEL);
}

static void
_dma_handler(void)
{
        struct dma_queue *dma_queue;
        dma_queue = &_state.dma_queues[_state.current_tag];

        struct dma_queue_request *top_request;
        top_request = _queue_top(dma_queue);

        volatile struct dma_queue_request *current_request;
        current_request = top_request;

        current_request->transfer.status = DMA_QUEUE_STATUS_COMPLETE;

        top_request->handler(&top_request->transfer);

        if (_queue_empty(dma_queue)) {
                _state.current_tag = DMA_QUEUE_TAG_INVALID;
                return;
        }

        const struct dma_queue_request *next_request;
        next_request = _queue_dequeue(dma_queue);

        _dma_queue_request_start(next_request);
}

static void
_dma_illegal_handler(void)
{
        assert(false);
}

static void
_default_handler(const dma_queue_transfer_t *work __unused)
{
}

/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <string.h>

#include <cpu/cache.h>
#include <cpu/intc.h>
#include <cpu/registers.h>

#include <sys/dma-queue.h>

#include <scu-internal.h>

/* Default SCU DMA level */
#define DMA_QUEUE_SCU_DMA_LEVEL         (0)

/* Maximum count of requests in ring buffer to take in */
#define DMA_QUEUE_REQUESTS_MAX_COUNT    (16)
#define DMA_QUEUE_REQUESTS_MASK         (DMA_QUEUE_REQUESTS_MAX_COUNT - 1)

struct dma_queue_request {
        scu_dma_handle_t handle;

        dma_queue_request_hdl_t handler;
        dma_queue_transfer_t transfer;
} __aligned(8);

static_assert(sizeof(struct dma_queue_request) == 32);

struct dma_queue {
        struct dma_queue_request *requests;

        volatile int8_t head;
        volatile int8_t tail;
        volatile int8_t flush_tail;
} __aligned(16);

static_assert(sizeof(struct dma_queue) == 16);

static inline void _queue_init(const uint8_t, struct dma_queue *) __always_inline;
static inline uint32_t _queue_size(const struct dma_queue *) __always_inline;
static inline bool _queue_full(const struct dma_queue *) __always_inline;
static inline bool _queue_empty(const struct dma_queue *) __always_inline;
static inline struct dma_queue_request *_queue_top(struct dma_queue *) __always_inline;
static inline struct dma_queue_request *_queue_enqueue(struct dma_queue *) __always_inline;
static inline struct dma_queue_request *_queue_dequeue(struct dma_queue *) __always_inline;
static void _queue_request_start(const struct dma_queue_request *request);

static void _default_handler(const dma_queue_transfer_t *);

static struct dma_queue_request _dma_queue_request_pools[DMA_QUEUE_TAG_COUNT][DMA_QUEUE_REQUESTS_MAX_COUNT];
static struct dma_queue _dma_queues[DMA_QUEUE_TAG_COUNT];

static struct {
        volatile uint8_t current_tag;
        volatile scu_dma_level_t current_level;
        struct dma_queue *dma_queues;
} _state __aligned(4);

static inline void __always_inline
_queue_init(const uint8_t tag, struct dma_queue *dma_queue)
{
        dma_queue->requests = &_dma_queue_request_pools[tag][0];
        dma_queue->head = -1;
        dma_queue->tail = -1;
        dma_queue->flush_tail = -1;

        for (uint32_t i = 0; i < DMA_QUEUE_REQUESTS_MAX_COUNT; i++) {
                struct dma_queue_request *request;
                request = &dma_queue->requests[i];

                request->handler = _default_handler;

                request->transfer.status = DMA_QUEUE_STATUS_UNKNOWN;
                request->transfer.work = NULL;
        }
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

/* Determine if the queue is empty, relative to where the flush tail is. */
static inline bool __always_inline
_queue_flush_empty(const struct dma_queue *dma_queue)
{
        return (dma_queue->head == dma_queue->flush_tail);
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

        request->transfer.status = DMA_QUEUE_STATUS_UNPROCESSED;

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

        request->transfer.status &= ~DMA_QUEUE_STATUS_UNPROCESSED;
        request->transfer.status |= DMA_QUEUE_STATUS_PROCESSING;

        dma_queue->head++;

        return request;
}

static void
_queue_request_start(const struct dma_queue_request *request)
{
        scu_dma_level_t level;
        level = DMA_QUEUE_SCU_DMA_LEVEL;

        if ((scu_dma_level_busy(level))) {
                if ((level = scu_dma_level_unused_get()) < 0) {
                        level = 0;
                }
        }

        _state.current_level = level;

        scu_dma_config_set(level, SCU_DMA_START_FACTOR_ENABLE, &request->handle, NULL);

        cpu_cache_purge();

        scu_dma_level_fast_start(level);
}

void
_internal_dma_queue_init(void)
{
        _state.current_tag = DMA_QUEUE_TAG_INVALID;
        _state.dma_queues = &_dma_queues[0];

        for (uint32_t tag = 0; tag < DMA_QUEUE_TAG_COUNT; tag++) {
                struct dma_queue *dma_queue;
                dma_queue = &_state.dma_queues[tag];

                _queue_init(tag, dma_queue);
        }
}

int8_t
dma_queue_enqueue(const scu_dma_handle_t *handle, uint8_t tag,
    dma_queue_request_hdl_t handler,
    void *work)
{
        assert(handle != NULL);

        assert(tag < DMA_QUEUE_TAG_COUNT);

        int8_t status;
        status = 0;

        const uint32_t intc_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        struct dma_queue *dma_queue;
        dma_queue = &_state.dma_queues[tag];

        if (_queue_full(dma_queue)) {
                status = -1;

                goto exit;
        }

        struct dma_queue_request *request;
        request = _queue_enqueue(dma_queue);

        assert(request != NULL);

        (void)memcpy(&request->handle, handle, sizeof(scu_dma_handle_t));

        request->handler = (handler != NULL) ? handler : _default_handler;
        request->transfer.work = work;

exit:
        cpu_intc_mask_set(intc_mask);

        return status;
}

int8_t
dma_queue_simple_enqueue(uint8_t tag, void *dst, void *src, size_t len)
{
        static scu_dma_handle_t handle = {
                .dnad = 0x00000101,
                .dnmd = 0x00010100
        };

        handle.dnr = (uint32_t)src;
        handle.dnw = (uint32_t)dst;
        handle.dnc = len;

        return dma_queue_enqueue(&handle, tag, NULL, NULL);
}

void
dma_queue_tag_clear(uint8_t tag)
{
        assert(tag < DMA_QUEUE_TAG_COUNT);

        dma_queue_flush_wait();

        const uint32_t intc_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        struct dma_queue *dma_queue;
        dma_queue = &_state.dma_queues[tag];

        while (!_queue_empty(dma_queue)) {
                struct dma_queue_request *request;
                request = _queue_dequeue(dma_queue);

                request->transfer.status |= DMA_QUEUE_STATUS_CANCELED;

                request->handler(&request->transfer);
        }

        cpu_intc_mask_set(intc_mask);
}

void
dma_queue_clear(void)
{
        const uint32_t intc_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        for (uint32_t tag = 0; tag < DMA_QUEUE_TAG_COUNT; tag++) {
                dma_queue_tag_clear(tag);
        }

        cpu_intc_mask_set(intc_mask);
}

uint32_t
dma_queue_flush(uint8_t tag)
{
        assert(tag < DMA_QUEUE_TAG_COUNT);

        dma_queue_flush_wait();

        int8_t status;
        status = 0;

        uint32_t intc_mask;
        intc_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        struct dma_queue * const dma_queue =
            &_state.dma_queues[tag];

        if (_queue_empty(dma_queue)) {
                status = 0;

                goto exit;
        }

        /* Mark what is going to be flushed */
        dma_queue->flush_tail = dma_queue->tail;

        _state.current_tag = tag;

        struct dma_queue_request * const request =
            _queue_dequeue(dma_queue);

        request->handler(&request->transfer);

        _queue_request_start(request);

        if (intc_mask >= CPU_INTC_PRIORITY_LEVEL_2_DMA) {
                intc_mask = CPU_INTC_PRIORITY_SPRITE_END - 1;
        }

exit:
        cpu_intc_mask_set(intc_mask);

        return status;
}

void
dma_queue_flush_wait(void)
{
        while (true) {
                if (_state.current_tag == DMA_QUEUE_TAG_INVALID) {
                        return;
                }

                scu_dma_level_wait(_state.current_level);

                struct dma_queue *dma_queue;
                dma_queue = &_state.dma_queues[_state.current_tag];

                struct dma_queue_request *top_request;
                top_request = _queue_top(dma_queue);

                volatile struct dma_queue_request *current_request;
                current_request = top_request;

                current_request->transfer.status &= ~DMA_QUEUE_STATUS_PROCESSING;
                current_request->transfer.status |= DMA_QUEUE_STATUS_COMPLETE;

                top_request->handler(&top_request->transfer);

                if (_queue_flush_empty(dma_queue)) {
                        _state.current_tag = DMA_QUEUE_TAG_INVALID;

                        return;
                }

                const struct dma_queue_request * const next_request =
                    _queue_dequeue(dma_queue);

                uint32_t intc_mask;
                intc_mask = cpu_intc_mask_get();

                if (intc_mask >= CPU_INTC_PRIORITY_LEVEL_2_DMA) {
                        intc_mask = CPU_INTC_PRIORITY_SPRITE_END - 1;
                }

                cpu_intc_mask_set(intc_mask);

                _queue_request_start(next_request);
        }
}

uint32_t
dma_queue_count_get(uint8_t tag)
{
        assert(tag < DMA_QUEUE_TAG_COUNT);

        const uint32_t intc_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        struct dma_queue *dma_queue;
        dma_queue = &_state.dma_queues[tag];

        uint32_t size;
        size = _queue_size(dma_queue);

        cpu_intc_mask_set(intc_mask);

        return size;
}

uint32_t
dma_queue_capacity_get(void)
{
        return DMA_QUEUE_REQUESTS_MAX_COUNT;
}

static void
_default_handler(const dma_queue_transfer_t *work __unused)
{
}

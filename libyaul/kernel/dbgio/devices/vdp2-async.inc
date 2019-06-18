/* -*- mode: c -*- */

static struct scu_dma_reg_buffer _dma_reg_buffer;

static void _async_init(const dbgio_vdp2_t *);
static void _async_deinit(void);
static void _async_flush(void);

const dbgio_dev_ops_t _internal_dev_ops_vdp2_async = {
        .dev = DBGIO_DEV_VDP2_ASYNC,
        .default_params = &_default_params,
        .init = (void (*)(const void *))_async_init,
        .deinit = _async_deinit,
        .buffer = _shared_buffer,
        .flush = _async_flush
};

static void _cancel_dma_handler(const struct dma_queue_transfer *);
static void _flush_dma_handler(const struct dma_queue_transfer *);

static void _init_complete_callback(void *);

static inline void *_address_align(const void *, const uint16_t);

static void
_async_init(const dbgio_vdp2_t *params)
{
        struct {
                /* Holds transfers for font CPD and PAL */
                struct scu_dma_xfer xfer_tbl[2];

                struct scu_dma_reg_buffer reg_buffer;
        } *dma_font;

        _shared_init(params);

        if ((_dev_state->state & STATE_MASK_INITIALIZED) != 0x00) {
                return;
        }

        void *aligned;
        aligned = malloc(sizeof(*dma_font) + 32);
        assert(aligned != NULL);
        dma_font = _address_align(aligned, 32);
        assert(((uintptr_t)dma_font & 0x1F) == 0x00);

        struct scu_dma_level_cfg dma_level_cfg;

        dma_level_cfg.mode = SCU_DMA_MODE_INDIRECT;
        dma_level_cfg.xfer.indirect = &dma_font->xfer_tbl[0];
        dma_level_cfg.stride = SCU_DMA_STRIDE_2_BYTES;
        dma_level_cfg.update = SCU_DMA_UPDATE_NONE;

        /* Set up indirect SCU-DMA table entries */

        /* Font CPD */
        dma_font->xfer_tbl[0].len = FONT_4BPP_CPD_SIZE;
        dma_font->xfer_tbl[0].dst = (uint32_t)_dev_state->cp_table;
        dma_font->xfer_tbl[0].src = CPU_CACHE_THROUGH | (uint32_t)_dev_state->font.cpd_buffer;

        /* Font PAL */
        dma_font->xfer_tbl[1].len = FONT_4BPP_COLOR_COUNT * sizeof(color_rgb555_t);
        dma_font->xfer_tbl[1].dst = _dev_state->color_palette;
        dma_font->xfer_tbl[1].src = SCU_DMA_INDIRECT_TBL_END | CPU_CACHE_THROUGH | (uint32_t)params->font_pal;

        scu_dma_config_buffer(&dma_font->reg_buffer, &dma_level_cfg);

        int8_t ret;
        ret = dma_queue_enqueue(&dma_font->reg_buffer, DMA_QUEUE_TAG_VBLANK_IN,
            _cancel_dma_handler, aligned);
        assert(ret == 0);

        /* 64x32 page PND */
        dma_level_cfg.mode = SCU_DMA_MODE_DIRECT;
        dma_level_cfg.xfer.direct.len = _dev_state->page_size;
        dma_level_cfg.xfer.direct.dst = (uint32_t)_dev_state->page_base;
        dma_level_cfg.xfer.direct.src = CPU_CACHE_THROUGH | (uint32_t)&_dev_state->page_pnd[0];
        dma_level_cfg.stride = SCU_DMA_STRIDE_2_BYTES;
        dma_level_cfg.update = SCU_DMA_UPDATE_NONE;

        scu_dma_config_buffer(&_dma_reg_buffer, &dma_level_cfg);

        /* We're truly initialized once the user has made at least one call to
         * vdp_sync() */
        vdp_sync_user_callback_add(_init_complete_callback, aligned);

        _dev_state->state = STATE_PARTIALLY_INITIALIZED;
}

static void
_async_deinit(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & STATE_MASK_INITIALIZED) == 0x00) {
                return;
        }

        /* XXX: Check if we're partially initialized. If so, cancel the queued
         * DMA requests */

        free(_dev_state->page_pnd);
        free(_dev_state);

        _dev_state = NULL;
}

static void
_async_flush(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & STATE_MASK_INITIALIZED) == 0x00) {
                return;
        }

        if ((_dev_state->state & STATE_BUFFER_DIRTY) != STATE_BUFFER_DIRTY) {
                return;
        }

        if ((_dev_state->state & STATE_BUFFER_FLUSHING) == STATE_BUFFER_FLUSHING) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_FLUSHING;

        _scroll_screen_reset();

        int8_t ret;
        ret = dma_queue_enqueue(&_dma_reg_buffer, DMA_QUEUE_TAG_VBLANK_IN,
            _flush_dma_handler, NULL);
        assert(ret == 0);
}

static inline void * __always_inline
_address_align(const void *address, const uint16_t align)
{
        assert((align & (align - 1)) == 0x0000);

        /* XXX: Refactor { */
        uint32_t aligned_offset;
        aligned_offset = (((uint32_t)address + (align - 1)) & ~(align - 1)) - (uint32_t)address;

        return (void *)((uint32_t)address + aligned_offset);
        /* } */
}

static void
_cancel_dma_handler(const struct dma_queue_transfer *transfer)
{
        if (transfer->status == DMA_QUEUE_STATUS_COMPLETE) {
                return;
        }

        void *free_ptr;
        free_ptr = transfer->work;

        /* When a DMA request is canceled, it's called outside of any
         * internal interrupt handlers, so we're able to call free() */
        free(free_ptr);

        _dev_state->state = STATE_IDLE;
}

static void
_flush_dma_handler(const struct dma_queue_transfer *transfer)
{
        if (transfer->status == DMA_QUEUE_STATUS_COMPLETE) {
                uint8_t state_mask;
                state_mask = STATE_BUFFER_DIRTY |
                             STATE_BUFFER_FLUSHING |
                             STATE_BUFFER_FORCE_FLUSHING;

                _dev_state->state &= ~state_mask;

                return;
        }

        /* If the DMA request was canceled, then we should allow force
         * flush while blocking any more writes to the buffer */
        _dev_state->state |= STATE_BUFFER_FORCE_FLUSHING;
}

static void
_init_complete_callback(void *work)
{
        if ((_dev_state->state & STATE_PARTIALLY_INITIALIZED) == 0x00) {
                return;
        }

        /* Free up all buffers allocated during initialization */
        void *free_ptr;
        free_ptr = work;

        free(free_ptr);

        free(_dev_state->font.cpd_buffer);
        free(_dev_state->font.pal_buffer);

        _dev_state->font.cpd_buffer = NULL;
        _dev_state->font.pal_buffer = NULL;

        _dev_state->state &= ~STATE_PARTIALLY_INITIALIZED;
        _dev_state->state |= STATE_INITIALIZED;
}

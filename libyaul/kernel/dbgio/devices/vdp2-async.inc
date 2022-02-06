/* -*- mode: c -*- */

static scu_dma_handle_t _dma_handle;

static void _async_init(const dbgio_vdp2_t *);
static void _async_flush(void);

const struct dbgio_dev_ops __dev_ops_vdp2_async = {
        .dev            = DBGIO_DEV_VDP2_ASYNC,
        .default_params = &_default_params,
        .init           = (dev_ops_init_t)_async_init,
        .deinit         = _shared_deinit,
        .font_load      = _shared_font_load,
        .puts           = _shared_puts,
        .flush          = _async_flush
};

static void _flush_dma_handler(const dma_queue_transfer_t *);

static void
_async_init(const dbgio_vdp2_t *params)
{
        _shared_init(params);

        /* Return if we're already initialized */
        if ((_dev_state->state & STATE_INITIALIZED) == STATE_INITIALIZED) {
                return;
        }

        /* 64x32 page PND */
        const scu_dma_level_cfg_t dma_cfg = {
                .mode            = SCU_DMA_MODE_DIRECT,
                .xfer.direct.len = _dev_state->page_size,
                .xfer.direct.dst = (uint32_t)_dev_state->page_base,
                .xfer.direct.src = CPU_CACHE_THROUGH | (uint32_t)&_dev_state->page_pnd[0],
                .space           = SCU_DMA_SPACE_BUS_B,
                .stride          = SCU_DMA_STRIDE_2_BYTES,
                .update          = SCU_DMA_UPDATE_NONE
        };

        scu_dma_config_buffer(&_dma_handle, &dma_cfg);

        _dev_state->state = STATE_INITIALIZED;
}

static void
_async_flush(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & STATE_INITIALIZED) == 0x00) {
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

        int8_t ret __unused;
        ret = dma_queue_enqueue(&_dma_handle, DMA_QUEUE_TAG_VBLANK_IN,
            _flush_dma_handler, NULL);
        assert(ret == 0);
}

static void
_flush_dma_handler(const dma_queue_transfer_t *transfer)
{
        if ((transfer->status & DMA_QUEUE_STATUS_COMPLETE) == DMA_QUEUE_STATUS_COMPLETE) {
                const uint8_t state_mask = STATE_BUFFER_DIRTY |
                                           STATE_BUFFER_FLUSHING |
                                           STATE_BUFFER_FORCE_FLUSHING;

                _dev_state->state &= ~state_mask;

                return;
        }

        /* If the DMA request was canceled, then we should allow force flush
         * while blocking any more writes to the buffer */
        _dev_state->state |= STATE_BUFFER_FORCE_FLUSHING;
}

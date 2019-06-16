/* -*- mode: c -*- */

static struct {
        /* List of pointers to free */
        void *free_ptrs[1];
        /* VDP sync callback ID to remove */
        int8_t sync_cid;
} *_init_work;

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

        _init_work = malloc(sizeof(*_init_work));
        assert(_init_work != NULL);

        /* Align to a 32-byte boundary */
        /* XXX: Refactor { */
        void *aligned;
        aligned = malloc(sizeof(*dma_font) + 32);
        assert(aligned != NULL);
        _init_work->free_ptrs[0] = aligned;

        uint32_t aligned_offset;
        aligned_offset = (((uint32_t)aligned + 0x0000001F) & ~0x0000001F) - (uint32_t)aligned;
        dma_font = (void *)((uint32_t)aligned + aligned_offset);
        /* } */

        struct scu_dma_level_cfg dma_level_cfg;

        dma_level_cfg.mode = SCU_DMA_MODE_INDIRECT;
        dma_level_cfg.xfer.indirect = &dma_font->xfer_tbl[0];
        dma_level_cfg.stride = SCU_DMA_STRIDE_2_BYTES;
        dma_level_cfg.update = SCU_DMA_UPDATE_NONE;

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
            _cancel_dma_handler, NULL);
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
        _init_work->sync_cid = vdp_sync_user_callback_add(_init_complete_callback, NULL);

        /* Due to the 1BPP font being decompressed in cached H-WRAM, we need to
         * flush the cache as the DMA transfer accesses the uncached mirror
         * address to the decompressed 4BPP font, which could result in fetching
         * stale values not yet written back to H-WRAM */
        cpu_cache_purge();

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

static void
_cancel_dma_handler(const struct dma_queue_transfer *transfer)
{
        if (transfer->dqt_status == DMA_QUEUE_STATUS_COMPLETE) {
                return;
        }

        /* When a DMA request is canceled, it's called outside of any
         * internal interrupt handlers, so we're able to call free() */
        free(_init_work->free_ptrs[0]);

        /* Avoid finalizing device initialization */
        vdp_sync_user_callback_remove(_init_work->sync_cid);

        _dev_state->state = STATE_IDLE;
}

static void
_flush_dma_handler(const struct dma_queue_transfer *transfer)
{
        if (transfer->dqt_status == DMA_QUEUE_STATUS_COMPLETE) {
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
_init_complete_callback(void *work __unused)
{
        if ((_dev_state->state & STATE_PARTIALLY_INITIALIZED) == 0x00) {
                return;
        }

        /* Free up all buffers allocated during initialization */

        free(_init_work->free_ptrs[0]);

        free(_init_work);

        free(_dev_state->font.cpd_buffer);
        free(_dev_state->font.pal_buffer);

        _dev_state->font.cpd_buffer = NULL;
        _dev_state->font.pal_buffer = NULL;

        _dev_state->state &= ~STATE_PARTIALLY_INITIALIZED;
        _dev_state->state |= STATE_INITIALIZED;
}

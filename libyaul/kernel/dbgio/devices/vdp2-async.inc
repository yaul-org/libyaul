/* -*- mode: c -*- */

static struct {
        /* List of pointers to free */
        void *free_ptrs[2];
        /* VDP sync callback ID to remove */
        int8_t sync_cid;
} _init_work;

static struct scu_dma_reg_buffer _dma_reg_buffer;

static void _async_init(const dbgio_vdp2_t *);
static void _async_deinit(void);
static void _async_buffer(const char *);
static void _async_flush(void);

const dbgio_dev_ops_t _internal_dev_ops_vdp2_async = {
        .dev = DBGIO_DEV_VDP2_ASYNC,
        .default_params = &_default_params,
        .init = (void (*)(const void *))_async_init,
        .deinit = _async_deinit,
        .buffer = _async_buffer,
        .flush = _async_flush
};

static void _cancel_dma_handler(const struct dma_queue_transfer *);
static void _flush_dma_handler(const struct dma_queue_transfer *);

static void _init_complete(void *);

static void
_async_init(const dbgio_vdp2_t *params)
{
        struct {
                /* Holds transfers for font CPD and PAL */
                struct scu_dma_xfer xfer_tbl[2];

                struct scu_dma_reg_buffer reg_buffer;
        } *dma_font;

        assert(params != NULL);

        assert(params->font_cpd != NULL);
        assert(params->font_pal != NULL);

        assert(params->font_bg <= 15);
        assert(params->font_bg <= 15);

        assert((params->scrn == VDP2_SCRN_NBG0) ||
               (params->scrn == VDP2_SCRN_NBG1) ||
               (params->scrn == VDP2_SCRN_NBG2) ||
               (params->scrn == VDP2_SCRN_NBG3));

        assert((params->scrn != VDP2_SCRN_RBG0) &&
               (params->scrn != VDP2_SCRN_RBG1));

        assert(params->cpd_bank <= 3);
        /* XXX: Fetch the VRAM bank split configuration and determine the VRAM
         *      bank size */
        assert(params->cpd_offset < VDP2_VRAM_BSIZE_4);

        assert(params->pnd_bank <= 3);
        /* XXX: Determine the page size and check against the number of
         *      available offsets */

        /* There are 128 16-color banks, depending on CRAM mode */
        /* XXX: Fetch CRAM mode and check number of available 16-color banks */
        assert(params->cram_index < 128);

        if (_dev_state == NULL) {
                _dev_state = malloc(sizeof(dev_state_t));
                assert(_dev_state != NULL);

                (void)memset(_dev_state, 0x00, sizeof(dev_state_t));

                _dev_state->state = STATE_IDLE;
        }

        if ((_dev_state->state & (STATE_PARTIALLY_INITIALIZED | STATE_INITIALIZED)) != 0x00) {
                return;
        }

        _dev_state->page_size = VDP2_SCRN_CALCULATE_PAGE_SIZE_M(1 * 1, 1);
        _dev_state->page_width = VDP2_SCRN_CALCULATE_PAGE_WIDTH_M(1 * 1);
        _dev_state->page_height = VDP2_SCRN_CALCULATE_PAGE_HEIGHT_M(1 * 1);

        /* One page per plane */
        _dev_state->page_base = VDP2_VRAM_ADDR(params->pnd_bank,
            params->pnd_offset * _dev_state->page_size);

        _dev_state->cp_table = VDP2_VRAM_ADDR(params->cpd_bank, params->cpd_offset);
        _dev_state->color_palette = VDP2_CRAM_ADDR(params->cram_index << 3);

        struct vdp2_scrn_cell_format cell_format = {
                .scroll_screen = params->scrn,
                .cc_count = VDP2_SCRN_CCC_PALETTE_16,
                .character_size = 1 * 1,
                .pnd_size = 1, /* 1-word */
                .auxiliary_mode = 0,
                .cp_table = _dev_state->cp_table,
                .color_palette = _dev_state->color_palette,
                .plane_size = 1 * 1,
                .map_bases.plane_a = _dev_state->page_base,
                .map_bases.plane_b = _dev_state->page_base,
                .map_bases.plane_c = _dev_state->page_base,
                .map_bases.plane_d = _dev_state->page_base
        };

        vdp2_scrn_cell_format_set(&cell_format);

        /* Restricting the page to 64x32 avoids wasting space */
        _dev_state->page_size /= 2;

        /* PND value used to clear pages */
        _dev_state->pnd_clear = VDP2_SCRN_PND_CONFIG_0(
                cell_format.cp_table,
                cell_format.color_palette,
                /* vf = */ 0,
                /* hf = */ 0);

        if (_dev_state->page_pnd == NULL) {
                _dev_state->page_pnd = malloc(_dev_state->page_size);
        }
        assert(_dev_state->page_pnd != NULL);

        uint8_t *dec_cpd;
        dec_cpd = (uint8_t *)malloc(FONT_4BPP_SIZE);
        assert(dec_cpd != NULL);
        _init_work.free_ptrs[0] = dec_cpd;

        _font_1bpp_4bpp_decompress(dec_cpd, params->font_cpd, params->font_fg,
            params->font_bg);

        struct scu_dma_level_cfg dma_level_cfg;

        /* Align to a 32-byte boundary */
        /* XXX: Refactor { */
        void *aligned;
        aligned = malloc(sizeof(*dma_font) + 32);
        assert(aligned != NULL);
        _init_work.free_ptrs[1] = aligned;

        uint32_t aligned_offset;
        aligned_offset = (((uint32_t)aligned + 0x0000001F) & ~0x0000001F) - (uint32_t)aligned;
        dma_font = (void *)((uint32_t)aligned + aligned_offset);
        /* } */

        dma_level_cfg.mode = SCU_DMA_MODE_INDIRECT;
        dma_level_cfg.xfer.indirect = &dma_font->xfer_tbl[0];
        dma_level_cfg.stride = SCU_DMA_STRIDE_2_BYTES;
        dma_level_cfg.update = SCU_DMA_UPDATE_NONE;

        /* Font CPD */
        dma_font->xfer_tbl[0].len = FONT_4BPP_SIZE;
        dma_font->xfer_tbl[0].dst = (uint32_t)_dev_state->cp_table;
        dma_font->xfer_tbl[0].src = CPU_CACHE_THROUGH | (uint32_t)dec_cpd;

        /* Font PAL */
        dma_font->xfer_tbl[1].len = FONT_COLOR_COUNT * sizeof(color_rgb555_t);
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

        cons_init(&_cons_ops, CONS_COLS_MIN, CONS_ROWS_MIN);

        /* We're truly initialized once the user has made at least one call to
         * vdp_sync() */
        _init_work.sync_cid = vdp_sync_user_callback_add(_init_complete, NULL);

        /* Due to the 1BPP font being decompressed in cached H-WRAM, we need to
         * flush the cache as the DMA transfer accesses the uncached mirror
         * address to the decompressed 4BPP font, which could result in fetching
         * stale values not yet written back to H-WRAM */
        cpu_cache_purge();

        /* Copy user's set device parameters */
        (void)memcpy(&_params, params, sizeof(dbgio_vdp2_t));

        _dev_state->state = STATE_PARTIALLY_INITIALIZED;
}

static void
_async_deinit(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & (STATE_PARTIALLY_INITIALIZED | STATE_INITIALIZED)) == 0x00) {
                return;
        }

        free(_dev_state->page_pnd);
        free(_dev_state);

        _dev_state = NULL;
}

static void
_async_buffer(const char *buffer)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & (STATE_PARTIALLY_INITIALIZED | STATE_INITIALIZED)) == 0x00) {
                return;
        }

        /* It's the best we can do for now. If the current buffer is marked for
         * flushing, we have to silently drop any calls to write to the
         * buffer */
        uint8_t state_mask;
        state_mask = STATE_BUFFER_FLUSHING | STATE_BUFFER_FORCE_FLUSHING;

        if ((_dev_state->state & state_mask) == STATE_BUFFER_FLUSHING) {
                return;
        }

        cons_buffer(buffer);
}

static void
_async_flush(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & (STATE_PARTIALLY_INITIALIZED | STATE_INITIALIZED)) == 0x00) {
                return;
        }

        if ((_dev_state->state & STATE_BUFFER_DIRTY) != STATE_BUFFER_DIRTY) {
                return;
        }

        if ((_dev_state->state & STATE_BUFFER_FLUSHING) == STATE_BUFFER_FLUSHING) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_FLUSHING;

        /* Force reset */
        vdp2_scrn_priority_set(_params.scrn, 7);
        vdp2_scrn_scroll_x_set(_params.scrn, F16(0.0f));
        vdp2_scrn_scroll_y_set(_params.scrn, F16(0.0f));
        vdp2_scrn_display_set(_params.scrn, /* transparent = */ true);

        vdp2_vram_cycp_bank_set(_params.cpd_bank, &_params.cpd_cycp);
        vdp2_vram_cycp_bank_set(_params.pnd_bank, &_params.pnd_cycp);

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
        free(_init_work.free_ptrs[0]);
        free(_init_work.free_ptrs[1]);

        /* Avoid finalizing device initialization */
        vdp_sync_user_callback_remove(_init_work.sync_cid);

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
_init_complete(void *work __unused)
{
        if ((_dev_state->state & STATE_PARTIALLY_INITIALIZED) == 0x00) {
                return;
        }

        /* Free up all buffers allocated during initialization */

        free(_init_work.free_ptrs[0]);
        free(_init_work.free_ptrs[1]);

        _init_work.free_ptrs[0] = NULL;
        _init_work.free_ptrs[1] = NULL;

        _dev_state->state &= ~STATE_PARTIALLY_INITIALIZED;
        _dev_state->state |= STATE_INITIALIZED;
}

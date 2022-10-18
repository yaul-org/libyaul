/* -*- mode: c -*- */

static void _async_init(const dbgio_vdp2_t *params);
static void _async_flush(void);

static void _flush_dma_handler(void *work);

const dbgio_dev_ops_t __dev_ops_vdp2_async = {
        .dev            = DBGIO_DEV_VDP2_ASYNC,
        .default_params = &_default_params,
        .init           = (dev_ops_init_t)_async_init,
        .deinit         = _shared_deinit,
        .font_load      = _shared_font_load,
        .puts           = _shared_puts,
        .flush          = _async_flush
};

static void
_async_init(const dbgio_vdp2_t *params)
{
        _shared_init(params);

        /* Return if we're already initialized */
        if ((_dev_state->state & STATE_INITIALIZED) == STATE_INITIALIZED) {
                return;
        }

        _dev_state->state = STATE_INITIALIZED;
}

static void
_async_flush(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & STATE_INITIALIZED) != STATE_INITIALIZED) {
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

        vdp_dma_enqueue((void *)_dev_state->page_base, _dev_state->page_pnd, _dev_state->page_size);
        vdp_dma_callback_add(_flush_dma_handler, NULL);
}

static void
_flush_dma_handler(void *work __unused)
{
        const uint8_t state_mask = STATE_BUFFER_DIRTY |
                                   STATE_BUFFER_FLUSHING |
                                   STATE_BUFFER_FORCE_FLUSHING;

        _dev_state->state &= ~state_mask;
}

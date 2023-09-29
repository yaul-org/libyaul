/* -*- mode: c -*- */

static void _async_init(const dbgio_vdp2_t *params);
static void _async_flush(void);

static void _dma_handler_flush(void *work);

const dbgio_dev_ops_t __dbgio_dev_ops_vdp2_async = {
    .dev            = DBGIO_DEV_VDP2_ASYNC,
    .default_params = &_default_params,
    .init           = (dev_ops_init_t)_async_init,
    .deinit         = _shared_deinit,
    .display_set    = _shared_display_set,
    .font_load      = _shared_font_load,
    .puts           = _shared_puts,
    .printf         = _shared_printf,
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

    const vdp2_scrn_normal_map_t * const normal_map =
      _dev_state->params.normal_map;

    uint16_t * const plane = (uint16_t *)normal_map->plane_a;
    uint16_t * const page = plane;

    vdp_dma_enqueue(page, _dev_state->page_ptr, _dev_state->page_size);
    vdp_dma_callback_add(_dma_handler_flush, NULL);
}

static void
_dma_handler_flush(void *work __unused)
{
    const uint8_t state_mask = STATE_BUFFER_DIRTY |
                               STATE_BUFFER_FLUSHING |
                               STATE_BUFFER_FORCE_FLUSHING;

    _dev_state->state &= ~state_mask;
}

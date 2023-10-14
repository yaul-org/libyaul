/* -*- mode: c -*- */

static void _sync_init(const dbgio_vdp2_t *params);
static void _sync_flush(void);

const dbgio_dev_ops_t __dbgio_dev_ops_vdp2 = {
    .dev            = DBGIO_DEV_VDP2,
    .default_params = &_default_params,
    .init           = (dev_ops_init_t)_sync_init,
    .deinit         = _shared_deinit,
    .display_set    = _shared_display_set,
    .font_load      = _shared_font_load,
    .puts           = _shared_puts,
    .printf         = _shared_printf,
    .flush          = _sync_flush
};

static void
_sync_init(const dbgio_vdp2_t *params)
{
    _shared_init(params);

    /* Return if we're already initialized */
    if ((_dev_state->state & STATE_INITIALIZED) == STATE_INITIALIZED) {
        return;
    }

    _dev_state->state = STATE_INITIALIZED;
}

static void
_sync_flush(void)
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

    _dev_state->state |= STATE_BUFFER_FLUSHING;

    const vdp2_scrn_normal_map_t * const normal_map =
      _dev_state->params.normal_map;

    uint16_t * const plane = (uint16_t *)normal_map->plane_a;
    uint16_t * const page = plane;

    scu_dma_transfer(2, page, _dev_state->page_ptr, _dev_state->page_size);

    const uint8_t state_mask = STATE_BUFFER_DIRTY |
      STATE_BUFFER_FLUSHING |
      STATE_BUFFER_FORCE_FLUSHING;

    _dev_state->state &= ~state_mask;

    _dev_state->state |= STATE_INITIALIZED;
}

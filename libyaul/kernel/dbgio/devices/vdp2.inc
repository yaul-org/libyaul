/* -*- mode: c -*- */

static void _init(const dbgio_vdp2_t *);
static void _flush(void);

const struct dbgio_dev_ops _internal_dev_ops_vdp2 = {
        .dev            = DBGIO_DEV_VDP2,
        .default_params = &_default_params,
        .init           = (dev_ops_init_t)_init,
        .deinit         = _shared_deinit,
        .font_load      = _shared_font_load,
        .puts           = _shared_puts,
        .flush          = _flush
};

static void
_init(const dbgio_vdp2_t *params)
{
        _shared_init(params);

        /* Return if we're already initialized */
        if ((_dev_state->state & STATE_INITIALIZED) == STATE_INITIALIZED) {
                return;
        }

        _dev_state->state = STATE_INITIALIZED;
}

static void
_flush(void)
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

        _scroll_screen_reset();

        scu_dma_transfer(2, (void *)_dev_state->page_base, _dev_state->page_pnd, _dev_state->page_size);

        const uint8_t state_mask = STATE_BUFFER_DIRTY |
                                   STATE_BUFFER_FLUSHING |
                                   STATE_BUFFER_FORCE_FLUSHING;

        _dev_state->state &= ~state_mask;

        _dev_state->state |= STATE_INITIALIZED;
}

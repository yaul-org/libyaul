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
        if ((_dev_state->state & STATE_INITIALIZED) != 0x00) {
                return;
        }

        _dev_state->state = STATE_INITIALIZED;
}

static void
_flush(void)
{
        static cpu_dmac_cfg_t dmac_cfg = {
                .channel = DEV_DMAC_CHANNEL,
                .src_mode = CPU_DMAC_SOURCE_INCREMENT,
                .src = 0x00000000,
                .dst = 0x00000000,
                .dst_mode = CPU_DMAC_DESTINATION_INCREMENT,
                .len = 0x00000000,
                .stride = CPU_DMAC_STRIDE_2_BYTES,
                .bus_mode = CPU_DMAC_BUS_MODE_CYCLE_STEAL,
                .ihr = NULL
        };

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

        dmac_cfg.len = _dev_state->page_size;
        dmac_cfg.dst = (uint32_t)_dev_state->page_base;
        dmac_cfg.src = CPU_CACHE_THROUGH | (uint32_t)&_dev_state->page_pnd[0];

        cpu_dmac_channel_wait(DEV_DMAC_CHANNEL);
        cpu_dmac_channel_config_set(&dmac_cfg);

        cpu_dmac_channel_start(DEV_DMAC_CHANNEL);
        cpu_dmac_channel_wait(DEV_DMAC_CHANNEL);

        const uint8_t state_mask = STATE_BUFFER_DIRTY |
                                   STATE_BUFFER_FLUSHING |
                                   STATE_BUFFER_FORCE_FLUSHING;

        _dev_state->state &= ~state_mask;

        _dev_state->state |= STATE_INITIALIZED;
}

/* -*- mode: c -*- */

static void _simple_init(const dbgio_vdp2_t *);
static void _simple_deinit(void);
static void _simple_flush(void);

const struct dbgio_dev_ops _internal_dev_ops_vdp2_simple = {
        .dev = DBGIO_DEV_VDP2_SIMPLE,
        .default_params = &_default_params,
        .init = (void (*)(const void *))_simple_init,
        .deinit = _simple_deinit,
        .buffer = _shared_buffer,
        .flush = _simple_flush
};

static void
_simple_init(const dbgio_vdp2_t *params)
{
        _shared_init(params);

        if ((_dev_state->state & STATE_MASK_INITIALIZED) != 0x00) {
                return;
        }

        _dev_state->state = STATE_PARTIALLY_INITIALIZED;
}

static void
_simple_deinit(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & STATE_MASK_INITIALIZED) == 0x00) {
                return;
        }

        _internal_free(_dev_state->page_pnd);
        _internal_free(_dev_state);

        _dev_state = NULL;
}

static void
_simple_flush(void)
{
        static cpu_dmac_cfg_t dmac_cfg = {
                .channel= DEV_DMAC_CHANNEL,
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

        if ((_dev_state->state & STATE_MASK_INITIALIZED) == 0x00) {
                return;
        }

        if ((_dev_state->state & STATE_BUFFER_DIRTY) != STATE_BUFFER_DIRTY) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_FLUSHING;

        _scroll_screen_reset();

        if ((_dev_state->state & STATE_PARTIALLY_INITIALIZED) == STATE_PARTIALLY_INITIALIZED) {
                dmac_cfg.len = FONT_4BPP_CPD_SIZE;
                dmac_cfg.dst = (uint32_t)_dev_state->cp_table;
                dmac_cfg.src = CPU_CACHE_THROUGH | (uint32_t)_dev_state->font.cpd_buffer;

                cpu_dmac_channel_wait(DEV_DMAC_CHANNEL);
                cpu_dmac_channel_config_set(&dmac_cfg);

                cpu_dmac_channel_start(DEV_DMAC_CHANNEL);
                cpu_dmac_channel_wait(DEV_DMAC_CHANNEL);

                dmac_cfg.len = FONT_4BPP_COLOR_COUNT * sizeof(color_rgb1555_t);
                dmac_cfg.dst = _dev_state->color_palette;
                dmac_cfg.src = CPU_CACHE_THROUGH | (uint32_t)_dev_state->font.pal_buffer;

                cpu_dmac_channel_config_set(&dmac_cfg);
                cpu_dmac_channel_start(DEV_DMAC_CHANNEL);
                cpu_dmac_channel_wait(DEV_DMAC_CHANNEL);

                _internal_free(_dev_state->font.cpd_buffer);
                _internal_free(_dev_state->font.pal_buffer);

                _dev_state->font.cpd_buffer = NULL;
                _dev_state->font.pal_buffer = NULL;
        }

        dmac_cfg.len = _dev_state->page_size;
        dmac_cfg.dst = (uint32_t)_dev_state->page_base;
        dmac_cfg.src = CPU_CACHE_THROUGH | (uint32_t)&_dev_state->page_pnd[0];

        cpu_dmac_channel_wait(DEV_DMAC_CHANNEL);
        cpu_dmac_channel_config_set(&dmac_cfg);

        cpu_dmac_channel_start(DEV_DMAC_CHANNEL);
        cpu_dmac_channel_wait(DEV_DMAC_CHANNEL);

        uint8_t state_mask;
        state_mask = STATE_BUFFER_DIRTY |
                     STATE_BUFFER_FLUSHING |
                     STATE_BUFFER_FORCE_FLUSHING;

        _dev_state->state &= ~state_mask;

        _dev_state->state &= ~STATE_PARTIALLY_INITIALIZED;
        _dev_state->state |= STATE_INITIALIZED;
}

/* -*- mode: c -*- */

static void _simple_init(const dbgio_vdp2_t *);
static void _simple_deinit(void);
static void _simple_buffer(const char *);
static void _simple_flush(void);

const dbgio_dev_ops_t _internal_dev_ops_vdp2_simple = {
        .dev = DBGIO_DEV_VDP2_SIMPLE,
        .default_params = &_default_params,
        .init = (void (*)(const void *))_simple_init,
        .deinit = _simple_deinit,
        .buffer = _simple_buffer,
        .flush = _simple_flush
};

static void
_simple_init(const dbgio_vdp2_t *params)
{
}

static void
_simple_deinit(void)
{
}

static void
_simple_buffer(const char *buffer)
{
}

static void
_simple_flush(void)
{
}

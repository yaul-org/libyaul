#include <bup.h>

#define BUP_LIBRARY_ADDRESS 0x6000358UL
#define BUP_VECTOR_ADDRESS  0x6000354UL

typedef void (*bup_init_t)(void *, void *, bup_config_t [3]);

bup_stub_t *__bup_stub = NULL;

void
bup_init(bup_t *bup, bup_config_t configs[3])
{
        smpc_smc_resdisa_call();

        ((bup_init_t)MEMORY_READ(32, BUP_LIBRARY_ADDRESS))(bup->lib, bup->workarea, configs);

        __bup_stub = (bup_stub_t *)MEMORY_READ(32, BUP_VECTOR_ADDRESS);

        smpc_smc_resenab_call();
}

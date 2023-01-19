#include <bup.h>

#define BUP_LIBRARY_ADDRESS 0x6000358UL
#define BUP_VECTOR_ADDRESS  0x6000354UL

typedef void (*bup_init_t)(void *, void *, bup_devices_t *devices);

bup_stub_t *__bup_stub = NULL;

void
bup_init(bup_t *bup, bup_devices_t *devices)
{
        smpc_smc_resdisa_call();

        ((bup_init_t)MEMORY_READ(32, BUP_LIBRARY_ADDRESS))(bup->lib, bup->workarea, devices);

        /* This is a bit of a kludge, but to use bup_device_t, subtract by one
         * to denote that the device is not connected. */
        devices->internal.device--;
        devices->external.device--;
        devices->serial.device--;

        __bup_stub = (bup_stub_t *)MEMORY_READ(32, BUP_VECTOR_ADDRESS);

        smpc_smc_resenab_call();
}

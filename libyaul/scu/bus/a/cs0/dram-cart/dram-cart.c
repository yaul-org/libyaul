/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <dram-cart.h>

#include <dram-cart/map.h>

#define DRAM_CART_BANKS (4)

static dram_cart_id_t _id = DRAM_CART_ID_INVALID;
static void *_base = NULL;

static bool _detect_dram_cart(void);

void
dram_cart_init(void)
{
        /* Write to A-Bus "dummy" area */
        MEMORY_WRITE(16, DUMMY(UNKNOWN), 0x0001);

        /* Set the SCU wait */
        /* Don't ask about this magic constant */
        const uint32_t asr0_bits = MEMORY_READ(32, SCU(ASR0));
        MEMORY_WRITE(32, SCU(ASR0), 0x23301FF0);

        /* Write to A-Bus refresh */
        const uint32_t aref_bits = MEMORY_READ(32, SCU(AREF));
        MEMORY_WRITE(32, SCU(AREF), 0x00000013);

        /* Determine ID and base address */
        if (!(_detect_dram_cart())) {
                /* Restore values in case we can't detect DRAM
                 * cartridge */
                MEMORY_WRITE(32, SCU(ASR0), asr0_bits);
                MEMORY_WRITE(32, SCU(AREF), aref_bits);
        }
}

void *
dram_cart_area_get(void)
{
        return _base;
}

dram_cart_id_t
dram_cart_id_get(void)
{
        return _id;
}

size_t
dram_cart_size_get(void)
{
        switch (_id) {
        case DRAM_CART_ID_1MIB:
                return 0x00100000;
        case DRAM_CART_ID_4MIB:
                return 0x00400000;
        default:
                return 0;
        }
}

static bool
_detect_dram_cart(void)
{
        /* Check the ID */
        _id = MEMORY_READ(8, CS1(ID));
        _id &= 0xFF;

        if ((_id != DRAM_CART_ID_1MIB) && (_id != DRAM_CART_ID_4MIB)) {
                _id = DRAM_CART_ID_INVALID;
                _base = NULL;

                return false;
        }

        uint32_t b;
        for (b = 0; b < DRAM_CART_BANKS; b++) {
                MEMORY_WRITE(32, DRAM(0, b, 0x00000000), 0x00000000);
                MEMORY_WRITE(32, DRAM(1, b, 0x00000000), 0x00000000);
        }

        /* Check DRAM #0 for mirrored banks */
        uint32_t write;
        write = 0x5A5A5A5A;
        MEMORY_WRITE(32, DRAM(0, 0, 0x00000000), write);

        for (b = 1; b < DRAM_CART_BANKS; b++) {
                uint32_t read;
                read = MEMORY_READ(32, DRAM(0, b, 0x00000000));

                /* Is it mirrored? */
                if (read != write) {
                        continue;
                }

                /* Thanks to Joe Fenton or the suggestion to return the
                 * last mirrored DRAM #0 bank in order to get a
                 * contiguous address space */
                if (_id != DRAM_CART_ID_1MIB) {
                        _id = DRAM_CART_ID_1MIB;
                }

                _base = (void *)DRAM(0, 3, 0x00000000);

                return true;
        }

        if (_id != DRAM_CART_ID_4MIB) {
                _id = DRAM_CART_ID_4MIB;
        }

        _base = (void *)DRAM(0, 0, 0x00000000);

        return true;
}

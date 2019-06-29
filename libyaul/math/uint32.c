#include "uint32.h"

/* Performs an unsigned log-base2 on the specified unsigned integer and returns
 * the result */
uint32_t
uint32_log2(uint32_t value)
{
        if (value == 0) {
                return 0;
        }

        uint32_t result;
        result = 0;

        if (value >= (1 << 16)) {
                value >>= 16;
                result += 16;
        }

        if (value >= (1 << 8)) {
                value >>= 8;
                result += 8;
        }

        if (value >= (1 << 4)) {
                value >>= 4;
                result += 4;
        }

        if (value >= (1 << 2)) {
                value >>= 2;
                result += 2;
        }

        if (value >= (1 << 1)) {
                result += 1;
        }

        return result;
}

uint32_t
uint32_loop_log2(uint32_t value)
{
        uint32_t l;
        l = 0;

        while ((value >> l) > 1) {
                ++l;
        }

        return l;
}

uint32_t
uint32_pow2(uint32_t value)
{
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value++;

        return value;
}

/* Archive of the original code from fuzziqer. No license was provided with the
 * released files, although fuzziqer stated using the source freely was fine as
 * long as he's credited for the compression and decompression code. */

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdint.h>

void
bcl_prs_decompress(void *in, void *out)
{
        uint32_t r3, r5; /* 6 unnamed registers */
        uint32_t bitpos = 9; /* 4 named registers */
        uint8_t *src_ptr = (uint8_t *)in;
        uint8_t *dst_ptr = (uint8_t *)out;
        uint8_t current_byte;
        bool flag;
        int offset;
        uint32_t x, t; /* 2 placed variables */

        current_byte = src_ptr[0];
        src_ptr++;

        while (true) {
                bitpos--;

                if (bitpos == 0) {
                        current_byte = src_ptr[0];
                        bitpos = 8;
                        src_ptr++;
                }

                flag = current_byte & 1;
                current_byte = current_byte >> 1;

                if (flag) {
                        dst_ptr[0] = src_ptr[0];
                        src_ptr++;
                        dst_ptr++;
                        continue;
                }

                bitpos--;

                if (bitpos == 0) {
                        current_byte = src_ptr[0];
                        bitpos = 8;
                        src_ptr++;
                }

                flag = current_byte & 1;
                current_byte = current_byte >> 1;

                if (flag) {
                        r3 = src_ptr[0] & 0xFF;

                        offset = ((src_ptr[1] & 0xFF) << 8) | r3;

                        src_ptr += 2;

                        if (offset == 0) {
                                return;
                        }

                        r3 = r3 & 0x00000007;
                        r5 = (offset >> 3) | 0xFFFFE000;

                        if (r3 == 0) {
                                flag = 0;
                                r3 = src_ptr[0] & 0xFF;
                                src_ptr++;
                                r3++;
                        } else {
                                r3 += 2;
                        }

                        r5 += (uint32_t) dst_ptr;
                } else {
                        r3 = 0;

                        for (x = 0; x < 2; x++) {
                                bitpos--;

                                if (bitpos == 0) {
                                        current_byte = src_ptr[0];
                                        bitpos = 8;
                                        src_ptr++;
                                }

                                flag = current_byte & 1;
                                current_byte = current_byte >> 1;
                                offset = r3 << 1;
                                r3 = offset | flag;
                        }

                        offset = src_ptr[0] | 0xFFFFFF00;
                        r3 += 2;
                        src_ptr++;
                        r5 = offset + (uint32_t)dst_ptr;
                }

                if (r3 == 0) {
                        continue;
                }

                t = r3;

                for (x = 0; x < t; x++) {
                        dst_ptr[0] = * (uint8_t *)r5;
                        r5++;
                        r3++;
                        dst_ptr++;
                }
        }
}

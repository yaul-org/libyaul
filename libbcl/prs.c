#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdint.h>

void
bcl_prs_decompress(void *in, void *out)
{
        uint32_t r3,r5; // 6 unnamed registers
        uint32_t bitpos = 9; // 4 named registers
        uint8_t* sourceptr = (uint8_t*) in;
        uint8_t* destptr = (uint8_t*) out;
        uint8_t currentbyte;
        bool flag;
        int offset;
        uint32_t x,t; // 2 placed variables

        //printf("\n> decompressing\n");
        currentbyte = sourceptr[0];
        sourceptr++;

        while (true) {
                bitpos--;
                if (bitpos == 0) {
                        currentbyte = sourceptr[0];
                        bitpos = 8;
                        sourceptr++;
                }
                flag = currentbyte & 1;
                currentbyte = currentbyte >> 1;
                if (flag) {
                        destptr[0] = sourceptr[0];
                        //printf("> > > %08X->%08X byte\n",sourceptr - sourceptr_orig,destptr - destptr_orig);
                        sourceptr++;
                        destptr++;
                        continue;
                }
                bitpos--;
                if (bitpos == 0) {
                        currentbyte = sourceptr[0];
                        bitpos = 8;
                        sourceptr++;
                }
                flag = currentbyte & 1;
                currentbyte = currentbyte >> 1;
                if (flag) {
                        r3 = sourceptr[0] & 0xFF;
                        //printf("> > > > > first: %02X - ",sourceptr[0]); system("PAUSE");
                        offset = ((sourceptr[1] & 0xFF) << 8) | r3;
                        //printf("> > > > > second: %02X - ",sourceptr[1]); system("PAUSE");
                        sourceptr += 2;
                        if (offset == 0) {
                                return;
                        }
                        r3 = r3 & 0x00000007;
                        r5 = (offset >> 3) | 0xFFFFE000;
                        if (r3 == 0) {
                                flag = 0;
                                r3 = sourceptr[0] & 0xFF;
                                sourceptr++;
                                r3++;
                        } else {
                                r3 += 2;
                        }
                        r5 += (uint32_t) destptr;
                        //printf("> > > %08X->%08X ldat %08X %08X %s\n",sourceptr - sourceptr_orig,destptr - destptr_orig,r5 - (uint32_t) destptr,r3,flag ? "inline" : "extended");
                } else {
                        r3 = 0;
                        for (x = 0; x < 2; x++) {
                                bitpos--;
                                if (bitpos == 0) {
                                        currentbyte = sourceptr[0];
                                        bitpos = 8;
                                        sourceptr++;
                                }
                                flag = currentbyte & 1;
                                currentbyte = currentbyte >> 1;
                                offset = r3 << 1;
                                r3 = offset | flag;
                        }
                        offset = sourceptr[0] | 0xFFFFFF00;
                        r3 += 2;
                        sourceptr++;
                        r5 = offset + (uint32_t) destptr;
                        //printf("> > > %08X->%08X sdat %08X %08X\n",sourceptr - sourceptr_orig,destptr - destptr_orig,r5 - (uint32_t) destptr,r3);
                }
                if (r3 == 0) {
                        continue;
                }
                t = r3;
                for (x = 0; x < t; x++) {
                        destptr[0] = * (uint8_t *)r5;
                        r5++;
                        r3++;
                        destptr++;
                }
        }
}

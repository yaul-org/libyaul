#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
        uint8_t bitpos;
        //uint8_t controlbyte;
        uint8_t* controlbyteptr;
        uint8_t* srcptr_orig;
        uint8_t* dstptr_orig;
        uint8_t* srcptr;
        uint8_t* dstptr;
} PRS_COMPRESSOR;

void prs_put_control_bit(PRS_COMPRESSOR* pc,uint8_t bit)
{
        *pc->controlbyteptr = *pc->controlbyteptr >> 1;
        *pc->controlbyteptr |= ((!!bit) << 7);
        pc->bitpos++;
        if (pc->bitpos >= 8) {
                pc->bitpos = 0;
                pc->controlbyteptr = pc->dstptr;
                pc->dstptr++;
        }
}

void prs_put_control_bit_nosave(PRS_COMPRESSOR* pc,uint8_t bit)
{
        *pc->controlbyteptr = *pc->controlbyteptr >> 1;
        *pc->controlbyteptr |= ((!!bit) << 7);
        pc->bitpos++;
}

void prs_put_control_save(PRS_COMPRESSOR* pc)
{
        if (pc->bitpos >= 8) {
                pc->bitpos = 0;
                pc->controlbyteptr = pc->dstptr;
                pc->dstptr++;
        }
}

void prs_put_static_data(PRS_COMPRESSOR* pc,uint8_t data)
{
        *pc->dstptr = data;
        pc->dstptr++;
}

uint8_t prs_get_static_data(PRS_COMPRESSOR* pc)
{
        uint8_t data = *pc->srcptr;
        pc->srcptr++;
        return data;
}

////////////////////////////////////////////////////////////////////////////////

void prs_init(PRS_COMPRESSOR* pc,void* src,void* dst)
{
        pc->bitpos = 0;
        //pc->controlbyte = 0;
        pc->srcptr = (uint8_t*) src;
        pc->srcptr_orig = (uint8_t*) src;
        pc->dstptr = (uint8_t*) dst;
        pc->dstptr_orig = (uint8_t*) dst;
        pc->controlbyteptr = pc->dstptr;
        pc->dstptr++;
}

void prs_finish(PRS_COMPRESSOR* pc)
{
        //printf("> > > prs_finish[1]: %08X->%08X\n",pc->srcptr - pc->srcptr_orig,pc->dstptr - pc->dstptr_orig);
        prs_put_control_bit(pc,0);
        prs_put_control_bit(pc,1);
        //printf("> > > prs_finish[2]: %08X->%08X %d\n",pc->srcptr - pc->srcptr_orig,pc->dstptr - pc->dstptr_orig,pc->bitpos);
        //pc->controlbyte = pc->controlbyte << (8 - pc->bitpos);
        if (pc->bitpos != 0) {
                *pc->controlbyteptr = ((*pc->controlbyteptr << pc->bitpos) >> 8);
                //*pc->controlbyteptr = pc->controlbyte;
                //pc->dstptr++;
        }
        //printf("> > > prs_finish[3]: %08X->%08X\n",pc->srcptr - pc->srcptr_orig,pc->dstptr - pc->dstptr_orig);
        prs_put_static_data(pc,0);
        prs_put_static_data(pc,0);
        //printf("> > > prs_finish[4]: %08X->%08X\n",pc->srcptr - pc->srcptr_orig,pc->dstptr - pc->dstptr_orig);
}

void prs_rawbyte(PRS_COMPRESSOR* pc)
{
        prs_put_control_bit_nosave(pc,1);
        prs_put_static_data(pc,prs_get_static_data(pc));
        prs_put_control_save(pc);
}

void prs_shortcopy(PRS_COMPRESSOR* pc,int offset,uint8_t size)
{
        size -= 2;
        prs_put_control_bit(pc,0);
        prs_put_control_bit(pc,0);
        prs_put_control_bit(pc, (size >> 1) & 1);
        prs_put_control_bit_nosave(pc,size & 1);
        prs_put_static_data(pc,offset & 0xFF);
        prs_put_control_save(pc);
}

void prs_longcopy(PRS_COMPRESSOR* pc,int offset,uint8_t size)
{
        uint8_t byte1,byte2;
        if (size <= 9) {
                //offset = ((offset << 3) & 0xFFF8) | ((size - 2) & 7);
                prs_put_control_bit(pc,0);
                prs_put_control_bit_nosave(pc,1);
                prs_put_static_data(pc, ((offset << 3) & 0xF8) | ((size - 2) & 0x07));
                prs_put_static_data(pc, (offset >> 5) & 0xFF);
                prs_put_control_save(pc);
        } else {
                //offset = (offset << 3) & 0xFFF8;
                prs_put_control_bit(pc,0);
                prs_put_control_bit_nosave(pc,1);
                prs_put_static_data(pc, (offset << 3) & 0xF8);
                prs_put_static_data(pc, (offset >> 5) & 0xFF);
                prs_put_static_data(pc,size - 1);
                prs_put_control_save(pc);
                //printf("[%08X, %08X, %02X%02X %02X]",offset,size,(offset >> 5) & 0xFF,(offset << 3) & 0xF8,size - 1);
        }
}

void prs_copy(PRS_COMPRESSOR* pc,int offset,uint8_t size)
{
        /*bool err = false;
          if (((offset & 0xFFFFE000) != 0xFFFFE000) || ((offset & 0x1FFF) == 0))
          {
          printf("> > > error: bad offset: %08X\n",offset);
          err = true;
          }
          if ((size > 0xFF) || (size < 3))
          {
          printf("> > > error: bad size: %08X\n",size);
          err = true;
          }
          if (err) system("PAUSE"); */
        //else printf("> > > prs_copy: %08X->%08X @ %08X:%08X\n",pc->srcptr - pc->srcptr_orig,pc->dstptr - pc->dstptr_orig,offset,size);
        if ((offset > -0x100) && (size <= 5)) {
                printf("> > > %08X->%08X sdat %08X %08X\n",pc->srcptr - pc->srcptr_orig,pc->dstptr - pc->dstptr_orig,offset,size);
                prs_shortcopy(pc,offset,size);
        } else {
                printf("> > > %08X->%08X ldat %08X %08X\n",pc->srcptr - pc->srcptr_orig,pc->dstptr - pc->dstptr_orig,offset,size);
                prs_longcopy(pc,offset,size);
        }
        pc->srcptr += size;
}

////////////////////////////////////////////////////////////////////////////////

uint32_t prs_decompress(void* source,void* dest)    // 800F7CB0 through 800F7DE4 in mem
{
        uint32_t r0,r3,r5,r6,r9; // 6 unnamed registers
        uint32_t bitpos = 9; // 4 named registers
        uint8_t* sourceptr = (uint8_t*) source;
        uint8_t* sourceptr_orig = (uint8_t*) source;
        uint8_t* destptr = (uint8_t*) dest;
        uint8_t* destptr_orig = (uint8_t*) dest;
        uint8_t currentbyte;
        bool flag;
        int offset;
        uint32_t x,t; // 2 placed variables

        printf("\n> decompressing\n");
        currentbyte = sourceptr[0];
        sourceptr++;
        for (;;) {
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
                        printf("> > > %08X->%08X byte\n",sourceptr - sourceptr_orig,destptr - destptr_orig);
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
                                return (uint32_t)(destptr - destptr_orig);
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
                        printf("> > > %08X->%08X ldat %08X %08X %s\n",sourceptr - sourceptr_orig,destptr - destptr_orig,r5 - (uint32_t) destptr,r3,flag ? "inline" : "extended");
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
                        printf("> > > %08X->%08X sdat %08X %08X\n",sourceptr - sourceptr_orig,destptr - destptr_orig,r5 - (uint32_t) destptr,r3);
                }
                if (r3 == 0) {
                        continue;
                }
                t = r3;
                for (x = 0; x < t; x++) {
                        destptr[0] = * (uint8_t*) r5;
                        r5++;
                        r3++;
                        destptr++;
                }
        }
}

uint32_t prs_decompress_size(void* source)
{
        uint32_t r0,r3,r5,r6,r9; // 6 unnamed registers
        uint32_t bitpos = 9; // 4 named registers
        uint8_t* sourceptr = (uint8_t*) source;
        uint8_t* destptr = NULL;
        uint8_t* destptr_orig = NULL;
        uint8_t currentbyte,lastbyte;
        bool flag;
        int offset;
        uint32_t x,t; // 2 placed variables

        //printf("> %08X -> %08X: begin\n",sourceptr,destptr);
        currentbyte = sourceptr[0];
        //printf("> [ ] %08X -> %02X: command stream\n",sourceptr,currentbyte);
        sourceptr++;
        for (;;) {
                bitpos--;
                if (bitpos == 0) {
                        lastbyte = currentbyte = sourceptr[0];
                        bitpos = 8;
                        //printf("> [ ] %08X -> %02X: command stream\n",sourceptr,currentbyte);
                        sourceptr++;
                }
                flag = currentbyte & 1;
                currentbyte = currentbyte >> 1;
                if (flag) {
                        //printf("> [1] %08X -> %08X: %02X\n",sourceptr,destptr,*sourceptr);
                        sourceptr++;
                        destptr++;
                        continue;
                }
                //printf("> [0] extended\n");
                bitpos--;
                if (bitpos == 0) {
                        lastbyte = currentbyte = sourceptr[0];
                        bitpos = 8;
                        //printf("> [ ] %08X -> %02X: command stream\n",sourceptr,currentbyte);
                        sourceptr++;
                }
                flag = currentbyte & 1;
                currentbyte = currentbyte >> 1;
                if (flag) {
                        r3 = sourceptr[0];
                        offset = (sourceptr[1] << 8) | r3;
                        sourceptr += 2;
                        if (offset == 0) {
                                return (uint32_t)(destptr - destptr_orig);
                        }
                        r3 = r3 & 0x00000007;
                        r5 = (offset >> 3) | 0xFFFFE000;
                        if (r3 == 0) {
                                r3 = sourceptr[0];
                                sourceptr++;
                                r3++;
                        } else {
                                r3 += 2;
                        }
                        r5 += (uint32_t) destptr;
                        //printf("> > [1] %08X -> %08X: block copy (%d)\n",r5,destptr,r3);
                } else {
                        r3 = 0;
                        for (x = 0; x < 2; x++) {
                                bitpos--;
                                if (bitpos == 0) {
                                        lastbyte = currentbyte = sourceptr[0];
                                        bitpos = 8;
                                        //printf("> [ ] %08X -> %02X: command stream\n",sourceptr,currentbyte);
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
                        //printf("> > [0] %08X -> %08X: block copy (%d)\n",r5,destptr,r3);
                }
                if (r3 == 0) {
                        continue;
                }
                t = r3;
                //printf("> > [ ] copying %d bytes\n",t);
                for (x = 0; x < t; x++) {
                        r5++;
                        r3++;
                        destptr++;
                }
        }
}

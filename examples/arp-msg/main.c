#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <yaul.h>

static struct cons cons;

#define BUFFER_SIZE 128
#define TIMEOUT 0x100
#define BYTE_INIT 0x21
#define BYTE_NOT_INIT 0x12

typedef struct {
    volatile unsigned char mark[4];
    volatile unsigned short ptr_r;
    volatile unsigned short ptr_w;
    volatile unsigned char buf[BUFFER_SIZE];
}iobuf_t;

volatile iobuf_t iobuf={ { 0x12, 0x34, 0x56, 0x78 } , 0, 0, {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
} };

static void to_reset(void){
    unsigned i;
    for(i=0;i<BUFFER_SIZE;i++)
        iobuf.buf[i]=i;
    iobuf.ptr_w=0;
    iobuf.ptr_r=0;
    iobuf.mark[0]=BYTE_NOT_INIT;
}

static void to_write_r(void *data, unsigned len){
    if (iobuf.mark[0]==BYTE_INIT) {
        unsigned ptr_w=iobuf.ptr_w;
        volatile unsigned ptr_r=iobuf.ptr_r;
        if (((ptr_w+len))>BUFFER_SIZE){
            {
                register unsigned timeout=0;
                while(ptr_r!=ptr_w) {
                    arp_function_nonblock();
                    ptr_r=iobuf.ptr_r;
                    if (timeout>=TIMEOUT) {
                        to_reset();
                        return;
                    }
                    timeout++;
                }
            }
            memcpy((void *)&iobuf.buf[ptr_w],data,BUFFER_SIZE-ptr_w);
            len-=(BUFFER_SIZE-ptr_w);
            data=(void *)(((unsigned long)data)+(BUFFER_SIZE-ptr_w));
            ptr_w=0;
            memcpy((void *)&iobuf.buf[0],data,len);
            iobuf.ptr_w=len;
        }  else {
            {
                register unsigned timeout=0;
                while(ptr_r>ptr_w) {
                    arp_function_nonblock();
                    ptr_r=iobuf.ptr_r;
                    if (timeout>=TIMEOUT) {
                        to_reset();
                        return;
                    }
                    timeout++;
                }
            }
            memcpy((void *)&iobuf.buf[ptr_w],data,len);
            ptr_w+=len;
            iobuf.ptr_w=ptr_w;
        }
    }
}

static void to_write(void *data, unsigned len){
    while(len>(BUFFER_SIZE-1)) {
        to_write_r(data,(BUFFER_SIZE-1));
        len-=(BUFFER_SIZE-1);
        data=(void *)(((unsigned long)data)+(BUFFER_SIZE-1));
    }
    if (len)
        to_write_r(data,len);
}

int
main(void)
{
        char *arp_ver;
        char *text;
        unsigned n,k;
        uint16_t single_color[] = { 0x9C00 };

        vdp2_init();
        vdp2_scrn_back_screen_set(/* single_color = */ true, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            single_color, 0);

        smpc_init();

        cons_init(&cons, CONS_DRIVER_VDP2);

        cons_write(&cons, "\n[1;44m   *** ARP Communication messages ***     [m\n\n");

        if ((text = (char *)malloc(1024)) == NULL) {
                abort();
        }

        cons_write(&cons, "Initializing ARP...\n");
        arp_ver = arp_version();

        if (*arp_ver == '\0') {
                cons_write(&cons, "No ARP cartridge detected!\n");
                abort();
        }
        cons_write(&cons, arp_ver);
        free(arp_ver);

        cons_write(&cons, "\nReady...\n\n");

        n=0;
        k=0;
        while (true) {
                k++;
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                arp_function_nonblock();
                if (!(k&1)) {
                    sprintf(text,"MESSAGE %i\n",n++);
                    to_write(text,strlen(text));
                    cons_write(&cons, "[2;9H\n");
                    cons_write(&cons, "\n\n\n\n\n\n");
                    cons_write(&cons, text);
                }
        }
        free(text);

        return 0;
}

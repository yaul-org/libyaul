#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "arp.h"
#include "binary.h"

#define UPLOAD_BINARY
#define UPDATES
#define DEBUG

#define BUFFER_SIZE 128
#define PTR_R_OFFSET 4
#define PTR_W_OFFSET 6
#define BUFFER_OFFSET 8
#define BYTE_INIT 0x21
#define BYTE_NOT_INIT 0x12

static volatile int toclose=0;
static unsigned short ptr_r=0;
static unsigned short ptr_w=0;

static int find_offset(void){
    int i;
    for(i=0;i<(binary_size-BUFFER_SIZE-8);i++)
        if (binary_data[i]==0x12 && binary_data[i+1]==0x34 && binary_data[i+2]==0x56 && binary_data[i+3]==0x78 && binary_data[i+4]==0x00 && binary_data[i+5]==0x00 && binary_data[i+6]==0x00 && binary_data[i+7]==0x00 && binary_data[i+8]==0x00 && binary_data[i+9]==0x01){
            return i;
        }
    return -1;
}

static void mysig(int v){
    toclose=1;
}

int main(void)
{
    int offset=find_offset();
    if (offset<0){
        puts("Unable to find mark");
        return 1;
    }
    signal(SIGABRT,mysig);
    signal(SIGHUP,mysig);
    signal(SIGTERM,mysig);
    signal(SIGINT,mysig);
    if (arp_open()){
        unsigned addr=0x06004000;
        unsigned l;
#ifdef UPLOAD_BINARY
        l=arp_write((void *)&binary_data,addr,binary_size,1);
        arp_close();
        if (l!=binary_size)
            return 2;
        sleep(1);
        if (arp_open())
#else
        if (1)
#endif
        {
            unsigned char data[BUFFER_OFFSET];
            while(!toclose){
                int times=0;
                data[0]=BYTE_NOT_INIT;
                l=arp_read((void *)&data[0],addr+offset,8);
#ifdef DEBUG
                printf("HEAD READ %i: 0x%.2X 0x%.2X 0x%.2X 0x%.2X | 0x%.2X 0x%.2X 0x%.2X 0x%.2X\n",l,data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
#endif
                if (l==8){
                    if (data[0]==BYTE_NOT_INIT){
                        data[0]=BYTE_INIT;
                        data[PTR_R_OFFSET]=0;
                        data[PTR_R_OFFSET+1]=0;
                        l=arp_write((void *)&data[0],addr+offset,PTR_R_OFFSET+2,0);
#ifdef DEBUG
                        printf("INIT %s\n",l==(PTR_R_OFFSET+2)?"OK":"ERROR");fflush(stdout);
#endif
                    } else {
#ifdef UPDATES
                        unsigned char dst[BUFFER_SIZE+4];
                        ptr_r=(((unsigned)data[PTR_R_OFFSET])<<8)+((unsigned)data[PTR_R_OFFSET+1]);
                        ptr_w=(((unsigned)data[PTR_W_OFFSET])<<8)+((unsigned)data[PTR_W_OFFSET+1]);
#ifdef DEBUG
                        printf("PTR_R=%i, PTR_W=%i\n",(int)ptr_r,(int)ptr_w);
#endif
                        while(ptr_r!=ptr_w){
                            times++;
                            memset((void *)&dst,0,sizeof(dst));
                            if (ptr_r<ptr_w){
                                unsigned len=ptr_w-ptr_r;
                                l=arp_read((void *)&dst,addr+offset+BUFFER_OFFSET+ptr_r,len);
                                if (l==len){
                                    ptr_r=ptr_w;
                                } else {
                                    break;
                                }
                            } else {
                                unsigned len=BUFFER_SIZE-ptr_r;
                                l=arp_read((void *)&dst,addr+offset+BUFFER_OFFSET+ptr_r,len);
                                if (l==len){
                                    ptr_r=0;
                                } else {
                                    break;
                                }
                            }
                            if (dst[0])
                                printf("%s",(char *)&dst[0]);
                        }
                        data[PTR_R_OFFSET]=(ptr_r>>8)&0xff;
                        data[PTR_R_OFFSET+1]=(ptr_r&0xff);
                        l=arp_write((void *)&data[PTR_R_OFFSET],addr+offset+PTR_R_OFFSET,2,0);
#ifdef DEBUG
                        printf("UPDATE %s: PTR_R=%i\n",l==2?"OK":"ERROR",ptr_r);fflush(stdout);
#endif
#endif
                    }
                }
                if (!times)
                    sleep(1);
            }
            arp_close();
        } else {
            puts("Unable to reopen");
        }
    } else {
        puts("Unable to open");
    }
    puts("END");
    return 0;
}


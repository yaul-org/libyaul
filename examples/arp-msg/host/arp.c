#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "ftd2xx.h"


// #define USE_SLEEP
#define MAX_TIMEOUT (1024*32)
#define MAX_LEN 191

#define BAUD        375000
#define PARITY      FT_PARITY_NONE
#define DATA_BITS   FT_BITS_8
#define STOP_BITS   FT_STOP_BITS_2

#define DIR_B0(RD) (((RD)>>0)&0xFF)
#define DIR_B1(RD) (((RD)>>8)&0xFF)
#define DIR_B2(RD) (((RD)>>16)&0xFF)
#define DIR_B3(RD) (((RD)>>24)&0xFF)


static FT_HANDLE ftHandle=0;
static DWORD to_dwRxSize = 0;

int arp_open(void)
{
	FT_STATUS	ftStatus;
	char * 	pcBufLD[2];
	char 	cBufLD[1][64];
	int	iNumDevs = 0;
    int i;

    to_dwRxSize = 0;
	for(i = 0; i < 1; i++) {
		pcBufLD[i] = cBufLD[i];
	}
	pcBufLD[1] = NULL;
	
	ftStatus = FT_ListDevices(pcBufLD, &iNumDevs, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER);
	
	if(ftStatus != FT_OK) {
		printf("Error: FT_ListDevices(%d)\n", (int)ftStatus);
		return 0;
	}
    if (iNumDevs==0){
        puts("No such device");
        return 0;
    }
    if((ftStatus = FT_OpenEx(cBufLD[0], FT_OPEN_BY_SERIAL_NUMBER, &ftHandle)) != FT_OK){
        printf("Error FT_OpenEx(%d), device %d\n", (int)ftStatus, 0);
        printf("Use lsmod to check if ftdi_sio (and usbserial) are present.\n");
        printf("If so, unload them using rmmod, as they conflict with ftd2xx.\n");
        return 0;
    }
    if((ftStatus = FT_SetBaudRate(ftHandle, BAUD)) != FT_OK) {
        printf("Error FT_SetBaudRate(%d), cBufLD[i] = %s\n", (int)ftStatus, cBufLD[0]);
        return 0;
    }
    if((ftStatus = FT_SetDataCharacteristics(ftHandle, DATA_BITS, STOP_BITS, PARITY)) != FT_OK) {
        printf("Error FT_SetDataCharacteristics(%d), cBufLD[i] = %s\n", (int)ftStatus, cBufLD[0]);
        return 0;
    }
    return 1;
}

void arp_close(void)
{
    FT_Close(ftHandle);
    ftHandle=0;
}

static int arp_read_block(void *buf, unsigned addr, unsigned size, int headone)
{
	FT_STATUS	ftStatus;
	DWORD 	dwBytesWritten, dwBytesRead;
    unsigned char headBuf[9];
    unsigned timeout;
	DWORD	dwRxSize = 0;
// printf("arp_read_block: ADDR=%.8X, size=%i, head=%i, buf=%p\n",addr,size,headone,buf);

    headBuf[0]  = 0x5A;
    headBuf[1]  = 0x07;
    headBuf[2]  = headone<0?0x11:headone>0?0x21:0x01;
    headBuf[3]  = DIR_B3(addr);
    headBuf[4]  = DIR_B2(addr);
    headBuf[5]  = DIR_B1(addr);
    headBuf[6]  = DIR_B0(addr);
    headBuf[7]  = size;
    headBuf[8]  = headBuf[1] + headBuf[2] + headBuf[3] + headBuf[4] + headBuf[5] + headBuf[6] + headBuf[7];

    dwBytesWritten=0;
    ftStatus = FT_Write(ftHandle, headBuf, sizeof(headBuf), &dwBytesWritten);
    if (ftStatus != FT_OK) {
        printf("Error FT_Write(%d) for Head [writter=%i]\n", (int)ftStatus,(int)dwBytesWritten);
        return 0;
    }
    if (dwBytesWritten != sizeof(headBuf)) {
        printf("FT_Write only wrote %d (of %d) bytes for Head\n", (int)dwBytesWritten, (int)sizeof(headBuf));
        return 0;
    }
#ifdef USE_SLEEP
    sleep(1);
#endif
    timeout = 0;
    dwRxSize = 0;
    while ((dwRxSize < sizeof(headBuf)) && (ftStatus == FT_OK)) {
        ftStatus = FT_GetQueueStatus(ftHandle, &dwRxSize);
        if (dwRxSize==0 && ftStatus ==FT_OK)
            timeout++;
        if (timeout>MAX_TIMEOUT) {
            printf("FT_GetQueueStatus TIMEOUT! [size=%i]\n",dwRxSize);
            ftStatus=12345;
            dwRxSize = 0;
        }
    }
    if(ftStatus == FT_OK) {
        headBuf[1]=headBuf[2]=0;
        ftStatus = FT_Read(ftHandle, headBuf, sizeof(headBuf), &dwBytesRead);
        if (ftStatus != FT_OK) {
            printf("Error FT_Read(%d) for Head\n", (int)ftStatus);
            dwRxSize = 0;
        } else if (dwBytesRead != sizeof(headBuf)) {
            printf("FT_Read only read %d (of %d) bytes for Head\n", (int)dwBytesRead, (int)sizeof(headBuf));
            dwRxSize = 0;
        } else if (headBuf[1]>0x07 && headBuf[2]==0xFF) {
            unsigned to_read=headBuf[1]-7;
            dwRxSize = 0;
            timeout=0;
            while ((dwRxSize < to_read) && (ftStatus == FT_OK)) {
                ftStatus = FT_GetQueueStatus(ftHandle, &dwRxSize);
                if (timeout>MAX_TIMEOUT) {
                    printf("FT_GetQueueStatus TIMEOUT!\n");
                    ftStatus=12345;
                    dwRxSize = 0;
                }
            }
            if(ftStatus == FT_OK) {
                unsigned char *pcBufRead=(unsigned char *)buf;
                ftStatus = FT_Read(ftHandle, buf, to_read, &dwBytesRead);
                if (ftStatus != FT_OK) {
                    printf("Error FT_Read(%d)\n", (int)ftStatus);
                    dwRxSize = 0;
                } else if (dwBytesRead != to_read) {
                    printf("FT_Read only read %d (of %d) bytes\n", (int)dwBytesRead, (int)to_read);
                    dwRxSize = 0;
                } else {
                    unsigned j;
                    unsigned char chk, nchk=0;
                    chk=pcBufRead[to_read-1];
                    for(j=1;j<sizeof(headBuf);j++)
                        nchk+=headBuf[j];
                    for(j=0;j<(to_read-1);j++)
                        nchk+=pcBufRead[j];
                    for(j=(to_read-1);j>0;j--)
                        pcBufRead[j]=pcBufRead[j-1];
                    pcBufRead[0]=headBuf[sizeof(headBuf)-1];
                    if (chk!=nchk)
                        dwRxSize = 0;
                }
            } else {
                if (ftStatus!=12345)
                    printf("Error FT_GetQueueStatus(%d)\n", (int)ftStatus);	
            }
        } else {
            printf("Error FT_Read Request for Head\n");
        }
    } else {
        if (ftStatus!=12345)
            printf("Error FT_GetQueueStatus(%d) for Head\n", (int)ftStatus);	
    }
    return dwRxSize;
}

static int arp_write_block(void *buf, unsigned addr, unsigned size, int boot)
{
	FT_STATUS	ftStatus;
	DWORD 	dwBytesWritten, dwBytesRead;
    unsigned char headBuf[198];
    unsigned timeout;
	DWORD	dwRxSize = 9+size;
// printf("arp_write_block: ADDR=%.8X, size=%i, boot=%i, buf=%p\n",addr,size,boot,buf);

    headBuf[0]  = 0x5A;
    headBuf[1]  = 0x07 + size;
    headBuf[2]  = boot?0x19:0x09;
    headBuf[3]  = DIR_B3(addr);
    headBuf[4]  = DIR_B2(addr);
    headBuf[5]  = DIR_B1(addr);
    headBuf[6]  = DIR_B0(addr);
    headBuf[7]  = size;
    memcpy((void *)&headBuf[8],buf,size);
    headBuf[8+size]  = 0;
    for(dwBytesWritten=1;dwBytesWritten<(8+size);dwBytesWritten++)
        headBuf[8+size] += headBuf[dwBytesWritten];

    ftStatus = FT_Write(ftHandle, headBuf, dwRxSize, &dwBytesWritten);
    if (ftStatus != FT_OK) {
        printf("Error FT_Write(%d)\n", (int)ftStatus);
        return 0;
    }
    if (dwBytesWritten != dwRxSize) {
        printf("FT_Write only wrote %d (of %d) bytes\n", (int)dwBytesWritten, (int)dwRxSize);
        return 0;
    }
#ifdef USE_SLEEP
    sleep(1);
#endif
    timeout = 0;
    dwRxSize = 0;
    while ((dwRxSize < 9) && (ftStatus == FT_OK)) {
        ftStatus = FT_GetQueueStatus(ftHandle, &dwRxSize);
        if (dwRxSize==0 && ftStatus ==FT_OK)
            timeout++;
        if (timeout>MAX_TIMEOUT) {
            printf("FT_GetQueueStatus TIMEOUT! [size=%i]\n",(int)dwRxSize);
            ftStatus=12345;
            dwRxSize = 0;
        }
    }
    if(ftStatus == FT_OK) {
        headBuf[2]=headBuf[8]=0;
        ftStatus = FT_Read(ftHandle, headBuf, 9, &dwBytesRead);
        if (ftStatus != FT_OK) {
            printf("Error FT_Read(%d) for Head\n", (int)ftStatus);
            dwRxSize = 0;
        } else if (dwBytesRead != 9) {
            printf("FT_Read only read %d (of %d) bytes for Head\n", (int)dwBytesRead, 9);
            dwRxSize = 0;
        } else if (headBuf[2]==0xFF && headBuf[8]==0x06) {
            dwRxSize = size;
        }
    }
    return dwRxSize;
}

int arp_read(void *buf, unsigned addr, unsigned size)
{
    int readed=0;
    int headone=0;
    if (ftHandle){
        while(size){
            if (size<=MAX_LEN){
                if (headone){
                    readed+=arp_read_block(buf,addr,size,1);
                } else {
                    unsigned l=(size/2);
                    readed+=arp_read_block(buf,addr,l,0);
                    buf=(void *)(((unsigned long)buf)+l);
                    addr+=l;
                    size-=l;
                    readed+=arp_read_block(buf,addr,size,1);
                }
                size=0;
            } else {
                unsigned l=arp_read_block(buf,addr,MAX_LEN,headone);
                headone=-1;
                readed+=l;
                addr+=l;
                size-=l;
                if (l!=MAX_LEN)
                    break;
                buf=(void *)(((unsigned long)buf)+MAX_LEN);
            }
        }
    }
    return readed;
}

int arp_write(void *buf, unsigned addr, unsigned size, int boot)
{
    int wrote=0;
    if (ftHandle){
        while(size){
            if (size<=MAX_LEN){
                wrote+=arp_write_block(buf,addr,size,boot);
                size=0;
            } else {
                unsigned l=size-MAX_LEN;
                void *b=(void *)(((unsigned long)buf)+l);
                unsigned w=arp_write_block(b,addr+l,MAX_LEN,0);
                size-=w;
                wrote+=w;
                if (w!=MAX_LEN)
                    break;
            }
        }
    }
    return wrote;
}

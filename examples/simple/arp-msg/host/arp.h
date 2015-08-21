#ifndef ARP_H
#define ARP_H

int arp_open(void);
void arp_close(void);
int arp_read(void *buf, unsigned addr, unsigned size);
int arp_write(void *buf, unsigned addr, unsigned size, int boot);

#endif

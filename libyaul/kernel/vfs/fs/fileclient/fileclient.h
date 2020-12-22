/*
 * Copyright (c) 2020 Israel Jacquez
 * See LICENSE for details.
 */

#ifndef _FILECLIENT_H_
#define _FILECLIENT_H_

#include <usb-cart.h>

#include <sys/cdefs.h>

#define FILECLIENT_SECTOR_SIZE (2048)

__BEGIN_DECLS

extern void fileclient_sector_request(const char *filename, const uint32_t sector_offset, void *dst);
extern uint32_t fileclient_sector_count_request(const char *filename);
extern uint32_t fileclient_byte_size_request(const char *filename);

__END_DECLS

#endif /* _FILECLIENT_H_ */

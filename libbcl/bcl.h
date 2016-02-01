/*
 * Copyright (c) 2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef BCL_H_
#define BCL_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint32_t prs_decompress_size(void *);
void Huffman_Uncompress(uint8_t *, uint8_t *, uint32_t, uint32_t);
void LZ_Uncompress(uint8_t *, uint8_t *, uint32_t);
void RLE_Uncompress(uint8_t *, uint8_t *, uint32_t);
void Rice_Uncompress(void *, void *, uint32_t, uint32_t, int);
void SF_Uncompress(uint8_t *, uint8_t *, uint32_t, uint32_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* BCL_H_ */

/*
 * Copyright (c) 2021
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _BCL_H_
#define _BCL_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @defgroup BCL BCL

/// @addtogroup BCL
/// @{

/// @brief Decompress a block of data using RLE.
///
/// @param[in]  in       The input buffer.
/// @param[out] out      The output buffer.
/// @param      in_size  The size of the input buffer in bytes.
/// @param      out_size The size of the output buffer in bytes.
extern void bcl_huffman_decompress(uint8_t *in, uint8_t *out, uint32_t in_size, uint32_t out_size);

/// @brief Decompress a block of data using PRS.
///
/// @param[in]  in      The input buffer.
/// @param[out] out     The output buffer.
extern void bcl_prs_decompress(void *in, void *out);

/// @brief Decompress a block of data using LZ77.
///
/// @param[in]  in      The input buffer.
/// @param[out] out     The output buffer.
/// @param      in_size Size of input buffer in bytes.
extern void bcl_lz_decompress(uint8_t *in, uint8_t *out, uint32_t in_size);

/// @brief Decompress a block of data using RLE.
///
/// @param[in]  in      The input buffer.
/// @param[out] out     The output buffer.
/// @param      in_size The size of the input buffer in bytes.
extern void bcl_rle_decompress(uint8_t *in, uint8_t *out, uint32_t in_size);

/// @}

__END_DECLS

#endif /* _BCL_H_ */

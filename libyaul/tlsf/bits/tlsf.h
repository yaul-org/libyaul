/*
 * Copyright (c) 2011 Matthew Conte
 * See LICENSE for details.
 *
 * Matthew Conte <matt@baisoku.org>
 */

#ifndef _TLSF_BITS_H_
#define _TLSF_BITS_H_

#ifdef __cplusplus
#define TLSF_DECL inline
#else
#define TLSF_DECL static
#endif /* __cplusplus */

/*
 * Architecture-specific bit manipulation routines.
 *
 * TLSF achieves O(1) cost for malloc and free operations by limiting
 * the search for a free block to a free list of guaranteed size
 * adequate to fulfill the request, combined with efficient free list
 * queries using bitmasks and architecture-specific bit-manipulation
 * routines.
 *
 * Most modern processors provide instructions to count leading zeroes
 * in a word, find the lowest and highest set bit, etc. These specific
 * implementations will be used when available, falling back to a
 * reasonably efficient generic implementation.
 *
 * NOTE: TLSF spec relies on ffs/fls returning value 0..31.
 * ffs/fls return 1-32 by default, returning 0 for error.
*/

/*
 * GCC 3.4 and above have builtin support, specialized for architecture.
 * Some compilers masquerade as gcc; patchlevel test filters them
 * out.
*/
#if defined (__GNUC__) &&                                                     \
        (__GNUC__ > 3 ||                                                      \
        (__GNUC__ == 3 &&                                                     \
        __GNUC_MINOR__ >= 4)) &&                                              \
        defined (__GNUC_PATCHLEVEL__)
TLSF_DECL int
tlsf_ffs(uint32_t word)
{
        return __builtin_ffs(word) - 1;
}

TLSF_DECL int
tlsf_fls(uint32_t word)
{
        const int bit = word ? 32 - __builtin_clz(word) : 0;
        return bit - 1;
}
#endif

#undef TLSF_DECL
#endif /* !_TLSF_BITS_H_ */

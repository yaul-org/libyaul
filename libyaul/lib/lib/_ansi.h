/* Provide support for both ANSI and non-ANSI environments. */

#ifndef	_ANSIDECL_H_
#define	_ANSIDECL_H_

/* ISO C++ */

#ifdef __cplusplus
#if !(defined(_BEGIN_STD_C) && defined(_END_STD_C))
#define _BEGIN_STD_C extern "C" {
#define _END_STD_C  }
#define _NOTHROW __attribute__ ((__nothrow__))
#endif
#else
#define _BEGIN_STD_C
#define _END_STD_C
#define _NOTHROW
#endif /* __cplusplus */

/* The traditional meaning of 'extern inline' for GCC is not to emit the
 * function body unless the address is explicitly taken.  However this
 * behaviour is changing to match the C99 standard, which uses 'extern
 * inline' to indicate that the function body *must* be emitted.
 * Likewise, a function declared without either 'extern' or 'static'
 * defaults to extern linkage (C99 6.2.2p5), and the compiler may choose
 * whether to use the inline version or call the extern linkage version
 * (6.7.4p6).  If we are using GCC, but do not have the new behaviour,
 * we need to use extern inline; if we are using a new GCC with the
 * C99-compatible behaviour, or a non-GCC compiler (which we will have
 * to hope is C99, since there is no other way to achieve the effect of
 * omitting the function if it isn't referenced) we use 'static inline',
 * which c99 defines to mean more-or-less the same as the Gnu C 'extern
 * inline'.
 */
#if defined(__GNUC__) && !defined(__GNUC_STDC_INLINE__)
/* We're using GCC, but without the new C99-compatible behaviour.  */
#define _ELIDABLE_INLINE extern __inline__ _ATTRIBUTE ((__always_inline__))
#else
/* We're using GCC in C99 mode, or an unknown compiler which
  we just have to hope obeys the C99 semantics of inline.  */
#define _ELIDABLE_INLINE static __inline__
#endif

#define _NOINLINE               __attribute__ ((__noinline__))
#define _NOINLINE_STATIC        _NOINLINE static

#endif /* _ANSIDECL_H_ */

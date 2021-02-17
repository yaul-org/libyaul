/*-
 * Copyright (c) 1991, 1993
 * The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _SYS_CDEFS_H_
#define _SYS_CDEFS_H_

#include <stddef.h>

/// @defgroup GCC GCC
/// @defgroup GCC_ATTRIBUTES Attributes
/// @ingroup GCC

/* Testing against Clang-specific extensions */
#ifndef __has_attribute
#define __has_attribute(x)      0
#endif /* !__has_attribute */

#ifndef __has_extension
#define __has_extension __has_feature
#endif /* !__has_extension */

#ifndef __has_feature
#define __has_feature(x)        0
#endif /* !__has_feature */

#ifndef __has_include
#define __has_include(x)        0
#endif /* !__has_include */

#ifndef __has_builtin
#define __has_builtin(x)        0
#endif /* !__has_builtin */

#ifdef __cplusplus
#define __BEGIN_DECLS   extern "C" {
#define __END_DECLS     }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif /* __cplusplus */

/* The __CONCAT macro is used to concatenate parts of symbol names, e.g. with
 * "#define OLD(foo) __CONCAT(old,foo)", OLD(foo) produces oldfoo. The __CONCAT
 * macro is a bit tricky to use if it must work in non-ANSI mode -- there must
 * be no spaces between its arguments, and for nested __CONCAT's, all the
 * __CONCAT's must be at the left. __CONCAT can also concatenate double-quoted
 * strings produced by the __STRING macro, but this only works with ANSI C.
 *
 * __XSTRING is like __STRING, but it expands any macros in its argument first.
 * It is only available with ANSI C. */

#define __CONCAT1(x,y)  x ## y
#define __CONCAT(x,y)   __CONCAT1(x,y)
#define __STRING(x)     #x /* Stringify without expanding x */
#define __STRINGIFY(x)  #x /* Stringify without expanding x */
#define __XSTRING(x)    __STRING(x) /* Expand x, then stringify */

/* Compiler-dependent macros to help declare dead (non-returning) and pure (no
 * side effects) functions, and unused variables.
 *
 * They are null except for versions of gcc that are known to support the
 * features properly (old versions of gcc-2 supported the dead and pure features
 * in a different (wrong) way). If we do not provide an implementation for a
 * given compiler, let the compile fail if it is told to use a feature that we
 * cannot live without. */

/// @addtogroup GCC_ATTRIBUTES
/// @{

/// @def __aligned(x)
/// @brief Not yet documented.
#define __aligned(x)            __attribute__ ((__aligned__ (x)))
/// @def __alloc_size(x)
/// @brief Not yet documented.
#define __alloc_size(x)         __attribute__ ((__alloc_size__ (x)))
/// @def __always_inline
/// @brief Not yet documented.
#define __always_inline         __attribute__ ((__always_inline__))
/// @def __const
/// @brief Not yet documented.
#define __const                 __attribute__ ((__const__))
/// @def __dead2
/// @brief Not yet documented.
#define __dead2                 __attribute__ ((__noreturn__))
/// @def __fastcall
/// @brief Not yet documented.
#define __fastcall              __attribute__ ((__fastcall__))
/// @def __interrupt_handler
/// @brief Not yet documented.
#define __interrupt_handler     __attribute__ ((interrupt_handler))
/// @def __malloc_like
/// @brief Not yet documented.
#define __malloc_like           __attribute__ ((__malloc__))
/// @def __may_alias
/// @brief Not yet documented.
#define __may_alias             __attribute__ ((__may_alias__))
/// @def __noinline
/// @brief Not yet documented.
#define __noinline              __attribute__ ((__noinline__))
/// @def __nonnull(x)
/// @brief Not yet documented.
#define __nonnull(x)            __attribute__ ((__nonnull__ (x)))
/// @def __nonnull_all
/// @brief Not yet documented.
#define __nonnull_all           __attribute__ ((__nonnull__))
/// @def __noreturn
/// @brief Not yet documented.
#define __noreturn              __attribute__ ((noreturn))
/// @def __packed
/// @brief Not yet documented.
#define __packed                __attribute__ ((__packed__))
/// @def __pure
/// @brief Not yet documented.
#define __pure                  __attribute__ ((__pure__))
/// @def __pure2
/// @brief Not yet documented.
#define __pure2                 __attribute__ ((__const__))
/// @def __result_use_check
/// @brief Not yet documented.
#define __result_use_check      __attribute__ ((__warn_unused_result__))
/// @def __returns_twice
/// @brief Not yet documented.
#define __returns_twice         __attribute__ ((__returns_twice__))
/// @def __section(x)
/// @brief Not yet documented.
#define __section(x)            __attribute__ ((__section__ (x)))
/// @def __unused
/// @brief Not yet documented.
#define __unused                __attribute__ ((__unused__))
/// @def __used
/// @brief Not yet documented.
#define __used                  __attribute__ ((__used__))
/// @def __weak
/// @brief Not yet documented.
#define __weak                  __attribute__ ((__weak__))
/// @def __leaf
/// @brief Not yet documented.
#define __leaf                  __attribute__ ((leaf))
/// @def __no_reorder
/// @brief Not yet documented.
#define __no_reorder            __attribute__ ((no_reorder))
/// @def __hot
/// @brief Not yet documented.
#define __hot                   __attribute__ ((hot))

/// @def __alloc_align(yyy)
/// @brief Not yet documented.
#if __has_attribute(__alloc_align__)
#define __alloc_align(x)        __attribute__ ((__alloc_align__ (x)))
#else
/// @def __always_inline
#define __alloc_align(x)
#endif /* __has_attribute(__alloc_align__) */

/* This function attribute indicates that an argument in a call to the function
 * is expected to be an explicit NULL.
 *
 * The attribute is only valid on variadic functions. */
/// @def __null_sentinel
/// @brief Not yet documented.
#define __null_sentinel         __attribute__ ((__sentinel__))

/// @}

/* C99 Static array indices in function parameter declarations. Syntax such as:
 *
 *   void bar(int myArray[static 10]);
 *
 * is allowed in C99 but not in C++.
 *
 * Define __min_size appropriately so
 * headers using it can be compiled in either language. Use like this:
 *
 *   void bar(int myArray[__min_size(10)]); */
/// @private
#if !defined(__cplusplus) &&                                                   \
    (defined(__clang__)) &&                                                    \
    (!defined(__STDC_VERSION__) || (__STDC_VERSION__ >= 199901))
#define __min_size(x) static (x)
#else
#define __min_size(x) (x)
#endif

/* The identifier __func__ is implicitly declared by the translator as if,
 * immediately following the opening brace of each function definition, the
 * declaration:
 *
 *   static const char __func__[] = "function-name";
 *
 * appeared, where function-name is the name of the lexically-enclosing
 * function. This name is the unadorned name of the function. As an extension,
 * at file (or, in C++, namespace scope), __func__ evaluates to the empty
 * string. */
/// @private
#define __func__ NULL

/* GCC 2.95 provides `__restrict' as an extension to C90 to support the
 * C99-specific `restrict' type qualifier. We happen to use `__restrict' as a
 * way to define the `restrict' type qualifier without disturbing older software
 * that is unaware of C99 keywords. */
/// @private
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901) || defined(lint)
#define __restrict
#else
#define __restrict restrict
#endif

/* GNU C version 2.96 adds explicit branch prediction so that the CPU back-end
 * can hint the processor and also so that code blocks can be reordered such
 * that the predicted path sees a more linear flow, thus improving cache
 * behavior, etc.
 *
 * The following two macros provide us with a way to utilize this compiler
 * feature. Use __predict_true() if you expect the expression to evaluate to
 * true, and __predict_false() if you expect the expression to evaluate to
 * false.
 *
 * A few notes about usage:
 *   * Generally, __predict_false() error condition checks (unless
 *     you have some _strong_ reason to do otherwise, in which case
 *     document it), and/or __predict_true() `no-error' condition
 *     checks, assuming you want to optimize for the no-error case.
 *
 *   * Other than that, if you don't know the likelihood of a test
 *     succeeding from empirical or other `hard' evidence, don't
 *     make predictions.
 *
 *   * These are meant to be used in places that are run `a lot'.
 *     It is wasteful to make predictions in code that is run
 *     seldomly (e.g. at subsystem initialization time) as the
 *     basic block reordering that this affects can often generate
 *     larger code. */
/// @private
#define __predict_true(exp)     __builtin_expect((exp), 1)
/// @private
#define __predict_false(exp)    __builtin_expect((exp), 0)

/// @private
#define __alignof(x)                                                            \
        __offsetof(struct { char __a; x __b; }, __b)
/// @private
#define __offsetof(type, field)                                                 \
        offsetof(type, field)
/// @private
#define __rangeof(type, start, end)                                             \
        (__offsetof(type, end) - __offsetof(type, start))

/* Compiler-dependent macros to declare that functions take printf-like or
 * scanf-like arguments.
 *
 * They are null except for versions of GCC that are known to support the
 * features properly (old versions of GCC-2 didn't permit keeping the keywords
 * out of the application namespace). */
/// @private
#define __printflike(fmt_arg, first_vararg)                                     \
        __attribute__ ((__format__ (__printf__, fmt_arg, first_vararg)))
/// @private
#define __format_arg(fmt_arg)                                                   \
        __attribute__ ((__format_arg__ (fmt_arg)))

/// @private
#define __weak_alias(name, aliasname)                                          \
        extern __typeof (name) aliasname __attribute__ ((weak, alias (#name)))

#endif /* !_SYS_CDEFS_H_ */

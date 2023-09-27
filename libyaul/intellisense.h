/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#if defined(__INTELLISENSE__) || defined(__RESHARPER__)
#undef __alloc_size
#define __alloc_size(x)

#undef __const
#define __const

#undef __dead2
#define __dead2

#undef __fastcall
#define __fastcall

#undef __interrupt_handler
#define __interrupt_handler

#undef __malloc_like
#define __malloc_like

#undef __may_alias
#define __may_alias

#undef __noinline
#define __noinline

#undef __nonnull
#define __nonnull(x)

#undef __nonnull_all
#define __nonnull_all

#undef __pure
#define __pure

#undef __pure2
#define __pure2

#undef __result_use_check
#define __result_use_check

#undef __returns_twice
#define __returns_twice

#undef __section
#define __section(x)

#undef __leaf
#define __leaf

#undef __no_reorder
#define __no_reorder

#undef __hot
#define __hot

#undef __hidden
#define __hidden

#undef __alloc_align
#define __alloc_align(x)

#undef __null_sentinel
#define __null_sentinel

#if !defined(__cplusplus) &&                                                   \
    (defined(__clang__)) &&                                                    \
    (!defined(__STDC_VERSION__) || (__STDC_VERSION__ >= 199901))
#undef __min_size
#define __min_size(x) (x)
#endif

#undef __func__
#define __func__

#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901)
#undef __restrict
#define __restrict
#endif

#undef __predict_true
#define __predict_true(exp)

#undef __predict_false
#define __predict_false(exp)

#undef __alignof
#define __alignof(x)

#undef __offsetof
#define __offsetof(type, field)

#undef __rangeof
#define __rangeof(type, start, end)

#undef __printflike
#define __printflike(fmt_arg, first_vararg)

#undef __format_arg
#define __format_arg(fmt_arg)

#undef __weak_alias
#define __weak_alias(name, aliasname)

#undef __register
#define __register

#undef __function_name
#define __function_name ((char *)0)

#undef __declare_asm
#define __declare_asm(...)

#undef __BEGIN_ASM
#define __BEGIN_ASM                                                            \
    __PRAGMA(GCC diagnostic push)                                              \
    __PRAGMA(GCC diagnostic ignored "-Wuninitialized")                         \
    __PRAGMA(GCC diagnostic ignored "-Wunused-variable")                       \
    __PRAGMA(GCC diagnostic ignored "-Wunused-parameter")

#undef __END_ASM
#define __END_ASM                                                              \
    __PRAGMA(GCC diagnostic pop)

#undef static_assert
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
/* Avoid including stdbool.h, as that would cause side effects */
#define static_assert(...) _Static_assert(1, "")
#else
#define static_assert(...) typedef char __CONCAT(STATIC_ASSERT_FAILED_AT_LINE, \
    __CONCAT(_, __LINE__))[1]
#endif

#endif

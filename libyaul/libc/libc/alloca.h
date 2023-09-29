/*-
 * Written 2000, Werner Almesberger
 *         2001, Corinna Vinschen <vinschen@redhat.com> */

#ifndef _LIB_ALLOCA_H_
#define _LIB_ALLOCA_H_

#undef alloca
#define alloca(size) __builtin_alloca(size)

#endif /* _LIB_ALLOCA_H_ */

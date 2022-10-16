/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_WHICH_H_
#define _YAUL_CPU_WHICH_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @defgroup CPU_DUAL CPU Dual

/// @addtogroup CPU_DUAL
/// @{

/// ID for which CPU.
typedef enum cpu_which {
        /// ID for master CPU
        CPU_MASTER,
        /// ID for slave CPU.
        CPU_SLAVE
} cpu_which_t;

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_WHICH_H_ */

/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SMPC_MAP_H_
#define _YAUL_SMPC_MAP_H_

#include <scu/map.h>

/// The number of IREG I/O registers available.
#define SMPC_IREGS      7

/// The number of OREG I/O registers available.
#define SMPC_OREGS      32

/// @addtogroup MEMORY_MAP_AREAS
/// @{

/// @brief Access the SMPC 7 IREG I/O registers.
///
/// @param x The IREG number.
///
/// @warning Byte access writes only.
///
/// @see MEMORY_WRITE
#define IREG(x)         (0x20100001UL + ((x) << 1))

/// @brief Access the SMPC 32 OREG I/O registers.
///
/// @param x The OREG number.
///
/// @warning Byte access reads only.
///
/// @see MEMORY_READ
#define OREG(x)         (0x20100021UL + ((x) << 1))

/// @brief Access the SMPC I/O registers.
///
/// @param x The byte offset.
///
/// @see MEMORY_READ
/// @see MEMORY_WRITE
#define SMPC(x)         (0x20100000UL + (x))

/// @}

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_SMPC_IO_REGISTERS SMPC I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define COMREG          0x01FUL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SR              0x061UL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SF              0x063UL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PDR1            0x075UL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PDR2            0x077UL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DDR1            0x079UL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DDR2            0x07BUL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define IOSEL1          0x07DUL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define IOSEL2          0x07DUL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define EXLE1           0x07FUL

/// @brief SMPC I/O register.
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define EXLE2           0x07FUL

/// @}

#endif /* !_YAUL_SMPC_MAP_H_ */

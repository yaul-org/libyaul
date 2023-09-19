/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_CONFIG_H_
#define _MIC3D_CONFIG_H_

#ifndef POLYGON_COUNT
#define POLYGON_COUNT 512
#endif /* POLYGON_COUNT */

#ifndef POINTS_COUNT
#define POINTS_COUNT  (POLYGON_COUNT * 4)
#endif /* POINTS_COUNT */

#ifndef SORT_DEPTH
#define SORT_DEPTH    512
#endif /* SORT_DEPTH */

#ifndef CMDT_COUNT
#define CMDT_COUNT    2048
#endif /* CMDT_COUNT */

#endif /* _MIC3D_CONFIG_H_ */

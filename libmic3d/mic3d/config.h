/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_CONFIG_H_
#define _MIC3D_CONFIG_H_

/* Maximum number of command tables per frame */
#ifndef CONFIG_MIC3D_CMDT_COUNT
#define CONFIG_MIC3D_CMDT_COUNT   2048
#endif /* !CONFIG_MIC3D_CMDT_COUNT */

/* Maximum number of points per render call */
#ifndef CONFIG_MIC3D_POINTS_COUNT
#define CONFIG_MIC3D_POINTS_COUNT (CONFIG_MIC3D_CMDT_COUNT / 4)
#endif /* !CONFIG_MIC3D_POINTS_COUNT */

#endif /* !_MIC3D_CONFIG_H_ */

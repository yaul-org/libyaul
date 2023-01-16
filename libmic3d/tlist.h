#ifndef _MIC3D_TLIST_H_
#define _MIC3D_TLIST_H_

#include "list.h"

typedef struct tlist {
        list_t list;
} tlist_t;

void __tlist_init(void);

#endif /* _MIC3D_TLIST_H_ */

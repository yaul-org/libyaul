#ifndef MIC3D_TLIST_H
#define MIC3D_TLIST_H

#include "list.h"

typedef struct tlist {
        list_t list;
} tlist_t;

void __tlist_init(void);

#endif /* MIC3D_TLIST_H */

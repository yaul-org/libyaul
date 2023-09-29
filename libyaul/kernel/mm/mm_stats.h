#ifndef _YAUL_KERNEL_MM_MM_STATS_H_
#define _YAUL_KERNEL_MM_MM_STATS_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

typedef enum mm_stats_type {
    MM_STATS_TYPE_YAUL,
    MM_STATS_TYPE_USER
} mm_stats_type_t;

typedef struct mm_stats_walk_entry {
    /* Pool */
    mm_stats_type_t pool_type;
    uintptr_t pool_address;
    size_t pool_size;

    /* Per allocation entry */
    bool used; /* If the allocation is freed */
    uintptr_t address;
    size_t size;

    /* User defined work pointer */
    void *work;
} mm_stats_walk_entry_t;

typedef void (*mm_stats_walker_t)(const mm_stats_walk_entry_t *walk_entry);

void mm_stats_walk(mm_stats_walker_t walker, void *work);

__END_DECLS

#endif /* _YAUL_KERNEL_MM_MM_STATS_H_ */

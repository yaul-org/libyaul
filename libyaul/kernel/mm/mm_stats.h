#ifndef _YAUL_KERNEL_MM_MM_STATS_H_
#define _YAUL_KERNEL_MM_MM_STATS_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

typedef struct mm_stats_walk_entry {
        uintptr_t address;
        size_t size;
        bool used;
        void *work;
} mm_stats_walk_entry_t;

typedef void (*mm_stats_walker_t)(const mm_stats_walk_entry_t *walk_entry);

void mm_stats_walk(mm_stats_walker_t walker, void *work);
void mm_stats_yaul_walk(mm_stats_walker_t walker, void *work);

__END_DECLS

#endif /* _YAUL_KERNEL_MM_MM_STATS_H_ */

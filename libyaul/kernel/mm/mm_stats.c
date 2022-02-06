#include <sys/cdefs.h>

#include <mm/mm_stats.h>

void __weak
mm_stats_walk(mm_stats_walker_t walker, void *work)
{
        extern void __mm_stats_walk(mm_stats_walker_t walker, void *work);

        __mm_stats_walk(walker, work);
}

void
mm_stats_yaul_walk(mm_stats_walker_t walker, void *work)
{
        extern void __mm_stats_private_walk(mm_stats_walker_t walker, void *work);

        __mm_stats_private_walk(walker, work);
}

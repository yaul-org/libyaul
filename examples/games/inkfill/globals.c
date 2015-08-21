#include <yaul.h>

#include "globals.h"

struct smpc_peripheral_digital g_digital;

/* Frame counter */
volatile uint32_t g_frame_counter = 0;

void
sleep(uint32_t seconds)
{
        uint32_t start_frame_count;

        start_frame_count = g_frame_counter;
        while ((g_frame_counter - start_frame_count) < (60 * seconds));
}

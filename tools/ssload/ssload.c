#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "drivers.h"

int
main(int argc, char **argv)
{
        const struct device_driver *device;
        device = device_drivers[DEVICE_DRIVER_DATALINK];

        if ((device->init()) < 0) {
                return 1;
        }

        if (argc == 1) {
                (void)fprintf(stderr, "data-link binary\n");
                return 1;
        }

        int ret;
        ret = device->upload_file(argv[1], 0x26004000);

        device->shutdown();

        return (ret == 0) ? 0 : 1;
}

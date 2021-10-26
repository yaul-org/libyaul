/***************************************************************************
                          ftdi.c  -  description
                             -------------------
    begin                : Fri Apr 4 2003
    copyright            : (C) 2003-2010 by Intra2net AG
    email                : opensource@intra2net.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License           *
 *   version 2.1 as published by the Free Software Foundation;             *
 *                                                                         *
 ***************************************************************************/

/**
    \mainpage libftdi API documentation

    Library to talk to FTDI chips. You find the latest versions of libftdi at
    http://www.intra2net.com/en/developer/libftdi/

    The library is easy to use. Have a look at this short example:
    \include simple.c

    More examples can be found in the "examples" directory.
*/
/** \addtogroup libftdi */
/* @{ */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <usbi.h>

#include "ftdi.h"

/* stuff needed for async write */
#ifdef LIBFTDI_LINUX_ASYNC_MODE
#include <linux/usbdevice_fs.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define ftdi_error_return(code, str)                                           \
  do {                                                                         \
    ftdi->error_str = str;                                                     \
    return code;                                                               \
  } while (0);

/**
    Internal function to close usb device pointer.
    Sets ftdi->usb_dev to NULL.
    \internal

    \param ftdi pointer to ftdi_context

    \retval  zero if all is fine, otherwise error code from usb_close()
*/
static int ftdi_usb_close_internal(struct ftdi_context *ftdi) {
  int ret = 0;

  if (ftdi && ftdi->usb_dev) {
    ret = usb_close(ftdi->usb_dev);
    ftdi->usb_dev = NULL;
  }

  return ret;
}

/**
    Initializes a ftdi_context.

    \param ftdi pointer to ftdi_context

    \retval  0: all fine
    \retval -1: couldn't allocate read buffer

    \remark This should be called before all functions
*/
int ftdi_init(struct ftdi_context *ftdi) {
  unsigned int i;

  ftdi->usb_dev = NULL;
  ftdi->usb_read_timeout = 5000;
  ftdi->usb_write_timeout = 5000;

  ftdi->type = TYPE_BM; /* chip type */
  ftdi->baudrate = -1;
  ftdi->bitbang_enabled =
      0; /* 0: normal mode 1: any of the bitbang modes enabled */

  ftdi->readbuffer = NULL;
  ftdi->readbuffer_offset = 0;
  ftdi->readbuffer_remaining = 0;
  ftdi->writebuffer_chunksize = 4096;
  ftdi->max_packet_size = 0;

  ftdi->interface = 0;
  ftdi->index = 0;
  ftdi->in_ep = 0x02;
  ftdi->out_ep = 0x81;
  ftdi->bitbang_mode =
      1; /* when bitbang is enabled this holds the number of the mode  */

  ftdi->error_str = NULL;

#ifdef LIBFTDI_LINUX_ASYNC_MODE
  ftdi->async_usb_buffer_size = 10;
  if ((ftdi->async_usb_buffer = malloc(sizeof(struct usbdevfs_urb) *
                                       ftdi->async_usb_buffer_size)) == NULL)
    ftdi_error_return(-1, "out of memory for async usb buffer");

  /* initialize async usb buffer with unused-marker */
  for (i = 0; i < ftdi->async_usb_buffer_size; i++)
    ((struct usbdevfs_urb *)ftdi->async_usb_buffer)[i].usercontext =
        FTDI_URB_USERCONTEXT_COOKIE;
#else
  ftdi->async_usb_buffer_size = 0;
  ftdi->async_usb_buffer = NULL;
#endif

  ftdi->eeprom_size = FTDI_DEFAULT_EEPROM_SIZE;

  ftdi->module_detach_mode = AUTO_DETACH_SIO_MODULE;

  /* All fine. Now allocate the readbuffer */
  return ftdi_read_data_set_chunksize(ftdi, 4096);
}

/**
    Allocate and initialize a new ftdi_context

    \return a pointer to a new ftdi_context, or NULL on failure
*/
struct ftdi_context *ftdi_new(void) {
  struct ftdi_context *ftdi =
      (struct ftdi_context *)malloc(sizeof(struct ftdi_context));

  if (ftdi == NULL) {
    return NULL;
  }

  if (ftdi_init(ftdi) != 0) {
    free(ftdi);
    return NULL;
  }

  return ftdi;
}

/**
    Open selected channels on a chip, otherwise use first channel.

    \param ftdi pointer to ftdi_context
    \param interface Interface to use for FT2232C/2232H/4232H chips.

    \retval  0: all fine
    \retval -1: unknown interface
    \retval -2: USB device unavailable
*/
int ftdi_set_interface(struct ftdi_context *ftdi,
                       enum ftdi_interface interface) {
  if (ftdi == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  switch (interface) {
  case INTERFACE_ANY:
  case INTERFACE_A:
    /* ftdi_usb_open_desc cares to set the right index, depending on the found
     * chip */
    break;
  case INTERFACE_B:
    ftdi->interface = 1;
    ftdi->index = INTERFACE_B;
    ftdi->in_ep = 0x04;
    ftdi->out_ep = 0x83;
    break;
  case INTERFACE_C:
    ftdi->interface = 2;
    ftdi->index = INTERFACE_C;
    ftdi->in_ep = 0x06;
    ftdi->out_ep = 0x85;
    break;
  case INTERFACE_D:
    ftdi->interface = 3;
    ftdi->index = INTERFACE_D;
    ftdi->in_ep = 0x08;
    ftdi->out_ep = 0x87;
    break;
  default:
    ftdi_error_return(-1, "Unknown interface");
  }
  return 0;
}

/**
    Deinitializes a ftdi_context.

    \param ftdi pointer to ftdi_context
*/
void ftdi_deinit(struct ftdi_context *ftdi) {
  if (ftdi == NULL)
    return;

  ftdi_usb_close_internal(ftdi);

  if (ftdi->async_usb_buffer != NULL) {
    free(ftdi->async_usb_buffer);
    ftdi->async_usb_buffer = NULL;
  }

  if (ftdi->readbuffer != NULL) {
    free(ftdi->readbuffer);
    ftdi->readbuffer = NULL;
  }
}

/**
    Deinitialize and free an ftdi_context.

    \param ftdi pointer to ftdi_context
*/
void ftdi_free(struct ftdi_context *ftdi) {
  ftdi_deinit(ftdi);
  free(ftdi);
}

/**
    Use an already open libusb device.

    \param ftdi pointer to ftdi_context
    \param usb libusb usb_dev_handle to use
*/
void ftdi_set_usbdev(struct ftdi_context *ftdi, usb_dev_handle *usb) {
  if (ftdi == NULL)
    return;

  ftdi->usb_dev = usb;
}

/**
    Finds all ftdi devices on the usb bus. Creates a new ftdi_device_list which
    needs to be deallocated by ftdi_list_free() after use.

    \param ftdi pointer to ftdi_context
    \param devlist Pointer where to store list of found devices
    \param vendor Vendor ID to search for
    \param product Product ID to search for

    \retval >0: number of devices found
    \retval -1: usb_find_busses() failed
    \retval -2: usb_find_devices() failed
    \retval -3: out of memory
*/
int ftdi_usb_find_all(struct ftdi_context *ftdi,
                      struct ftdi_device_list **devlist, int vendor,
                      int product) {
  struct ftdi_device_list **curdev;
  struct usb_bus *bus;
  struct usb_device *dev;
  int count = 0;

  usb_init();
  if (usb_find_busses() < 0)
    ftdi_error_return(-1, "usb_find_busses() failed");
  if (usb_find_devices() < 0)
    ftdi_error_return(-2, "usb_find_devices() failed");

  curdev = devlist;
  *curdev = NULL;
  for (bus = usb_get_busses(); bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      if (dev->descriptor.idVendor == vendor &&
          dev->descriptor.idProduct == product) {
        *curdev =
            (struct ftdi_device_list *)malloc(sizeof(struct ftdi_device_list));
        if (!*curdev)
          ftdi_error_return(-3, "out of memory");

        (*curdev)->next = NULL;
        (*curdev)->dev = dev;

        curdev = &(*curdev)->next;
        count++;
      }
    }
  }

  return count;
}

/**
    Frees a usb device list.

    \param devlist USB device list created by ftdi_usb_find_all()
*/
void ftdi_list_free(struct ftdi_device_list **devlist) {
  struct ftdi_device_list *curdev, *next;

  for (curdev = *devlist; curdev != NULL;) {
    next = curdev->next;
    free(curdev);
    curdev = next;
  }

  *devlist = NULL;
}

/**
    Frees a usb device list.

    \param devlist USB device list created by ftdi_usb_find_all()
*/
void ftdi_list_free2(struct ftdi_device_list *devlist) {
  ftdi_list_free(&devlist);
}

/**
    Return device ID strings from the usb device.

    The parameters manufacturer, description and serial may be NULL
    or pointer to buffers to store the fetched strings.

    \note Use this function only in combination with ftdi_usb_find_all()
          as it closes the internal "usb_dev" after use.

    \param ftdi pointer to ftdi_context
    \param dev libusb usb_dev to use
    \param manufacturer Store manufacturer string here if not NULL
    \param mnf_len Buffer size of manufacturer string
    \param description Store product description string here if not NULL
    \param desc_len Buffer size of product description string
    \param serial Store serial string here if not NULL
    \param serial_len Buffer size of serial string

    \retval   0: all fine
    \retval  -1: wrong arguments
    \retval  -4: unable to open device
    \retval  -7: get product manufacturer failed
    \retval  -8: get product description failed
    \retval  -9: get serial number failed
    \retval -10: unable to close device
*/
int ftdi_usb_get_strings(struct ftdi_context *ftdi, struct usb_device *dev,
                         char *manufacturer, int mnf_len, char *description,
                         int desc_len, char *serial, int serial_len) {
  if ((ftdi == NULL) || (dev == NULL))
    return -1;

  if (!(ftdi->usb_dev = usb_open(dev)))
    ftdi_error_return(-4, usb_strerror());

  if (manufacturer != NULL) {
    if (usb_get_string_simple(ftdi->usb_dev, dev->descriptor.iManufacturer,
                              manufacturer, mnf_len) <= 0) {
      ftdi_usb_close_internal(ftdi);
      ftdi_error_return(-7, usb_strerror());
    }
  }

  if (description != NULL) {
    if (usb_get_string_simple(ftdi->usb_dev, dev->descriptor.iProduct,
                              description, desc_len) <= 0) {
      ftdi_usb_close_internal(ftdi);
      ftdi_error_return(-8, usb_strerror());
    }
  }

  if (serial != NULL) {
    if (usb_get_string_simple(ftdi->usb_dev, dev->descriptor.iSerialNumber,
                              serial, serial_len) <= 0) {
      ftdi_usb_close_internal(ftdi);
      ftdi_error_return(-9, usb_strerror());
    }
  }

  if (ftdi_usb_close_internal(ftdi) != 0)
    ftdi_error_return(-10, usb_strerror());

  return 0;
}

/**
 * Internal function to determine the maximum packet size.
 * \param ftdi pointer to ftdi_context
 * \param dev libusb usb_dev to use
 * \retval Maximum packet size for this device
 */
static unsigned int _ftdi_determine_max_packet_size(struct ftdi_context *ftdi,
                                                    struct usb_device *dev) {
  unsigned int packet_size;

  // Sanity check
  if (ftdi == NULL || dev == NULL)
    return 64;

  // Determine maximum packet size. Init with default value.
  // New hi-speed devices from FTDI use a packet size of 512 bytes
  // but could be connected to a normal speed USB hub -> 64 bytes packet size.
  if (ftdi->type == TYPE_2232H || ftdi->type == TYPE_4232H ||
      ftdi->type == TYPE_232H)
    packet_size = 512;
  else
    packet_size = 64;

  if (dev->descriptor.bNumConfigurations > 0 && dev->config) {
    struct usb_config_descriptor config = dev->config[0];

    if (ftdi->interface < config.bNumInterfaces) {
      struct usb_interface interface = config.interface[ftdi->interface];
      if (interface.num_altsetting > 0) {
        struct usb_interface_descriptor descriptor = interface.altsetting[0];
        if (descriptor.bNumEndpoints > 0) {
          packet_size = descriptor.endpoint[0].wMaxPacketSize;
        }
      }
    }
  }

  return packet_size;
}

/**
    Opens a ftdi device given by an usb_device.

    \param ftdi pointer to ftdi_context
    \param dev libusb usb_dev to use

    \retval  0: all fine
    \retval -3: unable to config device
    \retval -4: unable to open device
    \retval -5: unable to claim device
    \retval -6: reset failed
    \retval -7: set baudrate failed
    \retval -8: ftdi context invalid
*/
int ftdi_usb_open_dev(struct ftdi_context *ftdi, struct usb_device *dev) {
  int detach_errno = 0;
  int config_val = 1;

  if (ftdi == NULL)
    ftdi_error_return(-8, "ftdi context invalid");

  if (!(ftdi->usb_dev = usb_open(dev)))
    ftdi_error_return(-4, "usb_open() failed");

#ifdef LIBUSB_HAS_GET_DRIVER_NP
  // Try to detach ftdi_sio kernel module.
  // Returns ENODATA if driver is not loaded.
  //
  // The return code is kept in a separate variable and only parsed
  // if usb_set_configuration() or usb_claim_interface() fails as the
  // detach operation might be denied and everything still works fine.
  // Likely scenario is a static ftdi_sio kernel module.
  if (ftdi->module_detach_mode == AUTO_DETACH_SIO_MODULE) {
    if (usb_detach_kernel_driver_np(ftdi->usb_dev, ftdi->interface) != 0 &&
        errno != ENODATA)
      detach_errno = errno;
  }
#endif

#if defined(__WIN32__) || defined(__CYGWIN__)
  // set configuration (needed especially for windows)
  // tolerate EBUSY: one device with one configuration, but two interfaces
  //    and libftdi sessions to both interfaces (e.g. FT2232)

  if (dev->descriptor.bNumConfigurations > 0) {
    // libusb-win32 on Windows 64 can return a null pointer for a valid device
    if (dev->config)
      config_val = dev->config[0].bConfigurationValue;

    if (usb_set_configuration(ftdi->usb_dev, config_val) && errno != EBUSY) {
      ftdi_usb_close_internal(ftdi);
      if (detach_errno == EPERM) {
        ftdi_error_return(-8, "inappropriate permissions on device!");
      } else {
        ftdi_error_return(-3, "unable to set usb configuration. Make sure the "
                              "default FTDI driver is not in use");
      }
    }
  }
#endif

  if (usb_claim_interface(ftdi->usb_dev, ftdi->interface) != 0) {
    ftdi_usb_close_internal(ftdi);
    if (detach_errno == EPERM) {
      ftdi_error_return(-8, "inappropriate permissions on device!");
    } else {
      ftdi_error_return(-5, "unable to claim usb device. Make sure the default "
                            "FTDI driver is not in use");
    }
  }

  if (ftdi_usb_reset(ftdi) != 0) {
    ftdi_usb_close_internal(ftdi);
    ftdi_error_return(-6, "ftdi_usb_reset failed");
  }

  // Try to guess chip type
  // Bug in the BM type chips: bcdDevice is 0x200 for serial == 0
  if (dev->descriptor.bcdDevice == 0x400 ||
      (dev->descriptor.bcdDevice == 0x200 &&
       dev->descriptor.iSerialNumber == 0))
    ftdi->type = TYPE_BM;
  else if (dev->descriptor.bcdDevice == 0x200)
    ftdi->type = TYPE_AM;
  else if (dev->descriptor.bcdDevice == 0x500)
    ftdi->type = TYPE_2232C;
  else if (dev->descriptor.bcdDevice == 0x600)
    ftdi->type = TYPE_R;
  else if (dev->descriptor.bcdDevice == 0x700)
    ftdi->type = TYPE_2232H;
  else if (dev->descriptor.bcdDevice == 0x800)
    ftdi->type = TYPE_4232H;
  else if (dev->descriptor.bcdDevice == 0x900)
    ftdi->type = TYPE_232H;

  // Set default interface on dual/quad type chips
  switch (ftdi->type) {
  case TYPE_2232C:
  case TYPE_2232H:
  case TYPE_4232H:
    if (!ftdi->index)
      ftdi->index = INTERFACE_A;
    break;
  default:
    break;
  }

  // Determine maximum packet size
  ftdi->max_packet_size = _ftdi_determine_max_packet_size(ftdi, dev);

  if (ftdi_set_baudrate(ftdi, 9600) != 0) {
    ftdi_usb_close_internal(ftdi);
    ftdi_error_return(-7, "set baudrate failed");
  }

  ftdi_error_return(0, "all fine");
}

/**
    Opens the first device with a given vendor and product ids.

    \param ftdi pointer to ftdi_context
    \param vendor Vendor ID
    \param product Product ID

    \retval same as ftdi_usb_open_desc()
*/
int ftdi_usb_open(struct ftdi_context *ftdi, int vendor, int product) {
  return ftdi_usb_open_desc(ftdi, vendor, product, NULL, NULL);
}

/**
    Opens the first device with a given, vendor id, product id,
    description and serial.

    \param ftdi pointer to ftdi_context
    \param vendor Vendor ID
    \param product Product ID
    \param description Description to search for. Use NULL if not needed.
    \param serial Serial to search for. Use NULL if not needed.

    \retval  0: all fine
    \retval -1: usb_find_busses() failed
    \retval -2: usb_find_devices() failed
    \retval -3: usb device not found
    \retval -4: unable to open device
    \retval -5: unable to claim device
    \retval -6: reset failed
    \retval -7: set baudrate failed
    \retval -8: get product description failed
    \retval -9: get serial number failed
    \retval -10: unable to close device
*/
int ftdi_usb_open_desc(struct ftdi_context *ftdi, int vendor, int product,
                       const char *description, const char *serial) {
  return ftdi_usb_open_desc_index(ftdi, vendor, product, description, serial,
                                  0);
}

/**
    Opens the index-th device with a given, vendor id, product id,
    description and serial.

    \param ftdi pointer to ftdi_context
    \param vendor Vendor ID
    \param product Product ID
    \param description Description to search for. Use NULL if not needed.
    \param serial Serial to search for. Use NULL if not needed.
    \param index Number of matching device to open if there are more than one,
   starts with 0.

    \retval  0: all fine
    \retval -1: usb_find_busses() failed
    \retval -2: usb_find_devices() failed
    \retval -3: usb device not found
    \retval -4: unable to open device
    \retval -5: unable to claim device
    \retval -6: reset failed
    \retval -7: set baudrate failed
    \retval -8: get product description failed
    \retval -9: get serial number failed
    \retval -10: unable to close device
    \retval -11: ftdi context invalid
*/
int ftdi_usb_open_desc_index(struct ftdi_context *ftdi, int vendor, int product,
                             const char *description, const char *serial,
                             unsigned int index) {
  struct usb_bus *bus;
  struct usb_device *dev;
  char string[256];

  usb_init();

  if (usb_find_busses() < 0)
    ftdi_error_return(-1, "usb_find_busses() failed");
  if (usb_find_devices() < 0)
    ftdi_error_return(-2, "usb_find_devices() failed");

  if (ftdi == NULL)
    ftdi_error_return(-11, "ftdi context invalid");

  for (bus = usb_get_busses(); bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      if (dev->descriptor.idVendor == vendor &&
          dev->descriptor.idProduct == product) {
        if (!(ftdi->usb_dev = usb_open(dev)))
          ftdi_error_return(-4, "usb_open() failed");

        if (description != NULL) {
          if (usb_get_string_simple(ftdi->usb_dev, dev->descriptor.iProduct,
                                    string, sizeof(string)) <= 0) {
            ftdi_usb_close_internal(ftdi);
            ftdi_error_return(-8, "unable to fetch product description");
          }
          if (strncmp(string, description, sizeof(string)) != 0) {
            if (ftdi_usb_close_internal(ftdi) != 0)
              ftdi_error_return(-10, "unable to close device");
            continue;
          }
        }
        if (serial != NULL) {
          if (usb_get_string_simple(ftdi->usb_dev,
                                    dev->descriptor.iSerialNumber, string,
                                    sizeof(string)) <= 0) {
            ftdi_usb_close_internal(ftdi);
            ftdi_error_return(-9, "unable to fetch serial number");
          }
          if (strncmp(string, serial, sizeof(string)) != 0) {
            if (ftdi_usb_close_internal(ftdi) != 0)
              ftdi_error_return(-10, "unable to close device");
            continue;
          }
        }

        if (ftdi_usb_close_internal(ftdi) != 0)
          ftdi_error_return(-10, "unable to close device");

        if (index > 0) {
          index--;
          continue;
        }

        return ftdi_usb_open_dev(ftdi, dev);
      }
    }
  }

  // device not found
  ftdi_error_return(-3, "device not found");
}

/**
    Opens the ftdi-device described by a description-string.
    Intended to be used for parsing a device-description given as commandline
   argument.

    \param ftdi pointer to ftdi_context
    \param description NULL-terminated description-string, using this format:
        \li <tt>d:\<devicenode></tt> path of bus and device-node (e.g.
   "003/001") within usb device tree (usually at /proc/bus/usb/) \li
   <tt>i:\<vendor>:\<product></tt> first device with given vendor and product
   id, ids can be decimal, octal (preceded by "0") or hex (preceded by "0x") \li
   <tt>i:\<vendor>:\<product>:\<index></tt> as above with index being the number
   of the device (starting with 0) if there are more than one \li
   <tt>s:\<vendor>:\<product>:\<serial></tt> first device with given vendor id,
   product id and serial string

    \note The description format may be extended in later versions.

    \retval  0: all fine
    \retval -1: usb_find_busses() failed
    \retval -2: usb_find_devices() failed
    \retval -3: usb device not found
    \retval -4: unable to open device
    \retval -5: unable to claim device
    \retval -6: reset failed
    \retval -7: set baudrate failed
    \retval -8: get product description failed
    \retval -9: get serial number failed
    \retval -10: unable to close device
    \retval -11: illegal description format
    \retval -12: ftdi context invalid
*/
int ftdi_usb_open_string(struct ftdi_context *ftdi, const char *description) {
  if (ftdi == NULL)
    ftdi_error_return(-12, "ftdi context invalid");

  if (description[0] == 0 || description[1] != ':')
    ftdi_error_return(-11, "illegal description format");

  if (description[0] == 'd') {
    struct usb_bus *bus;
    struct usb_device *dev;

    usb_init();

    if (usb_find_busses() < 0)
      ftdi_error_return(-1, "usb_find_busses() failed");
    if (usb_find_devices() < 0)
      ftdi_error_return(-2, "usb_find_devices() failed");

    for (bus = usb_get_busses(); bus; bus = bus->next) {
      for (dev = bus->devices; dev; dev = dev->next) {
        /* XXX: This doesn't handle symlinks/odd paths/etc... */
        const char *desc = description + 2;
        size_t len = strlen(bus->dirname);
        if (strncmp(desc, bus->dirname, len))
          continue;
        desc += len;
        if (desc[0] != '/')
          continue;
        ++desc;
        if (strcmp(desc, dev->filename))
          continue;
        return ftdi_usb_open_dev(ftdi, dev);
      }
    }

    // device not found
    ftdi_error_return(-3, "device not found");
  } else if (description[0] == 'i' || description[0] == 's') {
    unsigned int vendor;
    unsigned int product;
    unsigned int index = 0;
    const char *serial = NULL;
    const char *startp, *endp;

    errno = 0;
    startp = description + 2;
    vendor = strtoul((char *)startp, (char **)&endp, 0);
    if (*endp != ':' || endp == startp || errno != 0)
      ftdi_error_return(-11, "illegal description format");

    startp = endp + 1;
    product = strtoul((char *)startp, (char **)&endp, 0);
    if (endp == startp || errno != 0)
      ftdi_error_return(-11, "illegal description format");

    if (description[0] == 'i' && *endp != 0) {
      /* optional index field in i-mode */
      if (*endp != ':')
        ftdi_error_return(-11, "illegal description format");

      startp = endp + 1;
      index = strtoul((char *)startp, (char **)&endp, 0);
      if (*endp != 0 || endp == startp || errno != 0)
        ftdi_error_return(-11, "illegal description format");
    }
    if (description[0] == 's') {
      if (*endp != ':')
        ftdi_error_return(-11, "illegal description format");

      /* rest of the description is the serial */
      serial = endp + 1;
    }

    return ftdi_usb_open_desc_index(ftdi, vendor, product, NULL, serial, index);
  } else {
    ftdi_error_return(-11, "illegal description format");
  }
}

/**
    Resets the ftdi device.

    \param ftdi pointer to ftdi_context

    \retval  0: all fine
    \retval -1: FTDI reset failed
    \retval -2: USB device unavailable
*/
int ftdi_usb_reset(struct ftdi_context *ftdi) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE, SIO_RESET_REQUEST,
                      SIO_RESET_SIO, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "FTDI reset failed");

  // Invalidate data in the readbuffer
  ftdi->readbuffer_offset = 0;
  ftdi->readbuffer_remaining = 0;

  return 0;
}

/**
    Clears the read buffer on the chip and the internal read buffer.

    \param ftdi pointer to ftdi_context

    \retval  0: all fine
    \retval -1: read buffer purge failed
    \retval -2: USB device unavailable
*/
int ftdi_usb_purge_rx_buffer(struct ftdi_context *ftdi) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE, SIO_RESET_REQUEST,
                      SIO_RESET_PURGE_RX, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "FTDI purge of RX buffer failed");

  // Invalidate data in the readbuffer
  ftdi->readbuffer_offset = 0;
  ftdi->readbuffer_remaining = 0;

  return 0;
}

/**
    Clears the write buffer on the chip.

    \param ftdi pointer to ftdi_context

    \retval  0: all fine
    \retval -1: write buffer purge failed
    \retval -2: USB device unavailable
*/
int ftdi_usb_purge_tx_buffer(struct ftdi_context *ftdi) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE, SIO_RESET_REQUEST,
                      SIO_RESET_PURGE_TX, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "FTDI purge of TX buffer failed");

  return 0;
}

/**
    Clears the buffers on the chip and the internal read buffer.

    \param ftdi pointer to ftdi_context

    \retval  0: all fine
    \retval -1: read buffer purge failed
    \retval -2: write buffer purge failed
    \retval -3: USB device unavailable
*/
int ftdi_usb_purge_buffers(struct ftdi_context *ftdi) {
  int result;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-3, "USB device unavailable");

  result = ftdi_usb_purge_rx_buffer(ftdi);
  if (result < 0)
    return -1;

  result = ftdi_usb_purge_tx_buffer(ftdi);
  if (result < 0)
    return -2;

  return 0;
}

/**
    Closes the ftdi device. Call ftdi_deinit() if you're cleaning up.

    \param ftdi pointer to ftdi_context

    \retval  0: all fine
    \retval -1: usb_release failed
    \retval -2: usb_close failed
    \retval -3: ftdi context invalid
*/
int ftdi_usb_close(struct ftdi_context *ftdi) {
  int rtn = 0;

  if (ftdi == NULL)
    ftdi_error_return(-3, "ftdi context invalid");

#ifdef LIBFTDI_LINUX_ASYNC_MODE
  /* try to release some kernel resources */
  ftdi_async_complete(ftdi, 1);
#endif

  if (ftdi->usb_dev != NULL)
    if (usb_release_interface(ftdi->usb_dev, ftdi->interface) != 0)
      rtn = -1;

  if (ftdi_usb_close_internal(ftdi) != 0)
    rtn = -2;

  return rtn;
}

/**
    ftdi_convert_baudrate returns nearest supported baud rate to that requested.
    Function is only used internally
    \internal
*/
static int ftdi_convert_baudrate(int baudrate, struct ftdi_context *ftdi,
                                 unsigned short *value, unsigned short *index) {
  static const char am_adjust_up[8] = {0, 0, 0, 1, 0, 3, 2, 1};
  static const char am_adjust_dn[8] = {0, 0, 0, 1, 0, 1, 2, 3};
  static const char frac_code[8] = {0, 3, 2, 4, 1, 5, 6, 7};
  int divisor, best_divisor, best_baud, best_baud_diff;
  unsigned long encoded_divisor;
  int i;

  if (baudrate <= 0) {
    // Return error
    return -1;
  }

  divisor = 24000000 / baudrate;

  if (ftdi->type == TYPE_AM) {
    // Round down to supported fraction (AM only)
    divisor -= am_adjust_dn[divisor & 7];
  }

  // Try this divisor and the one above it (because division rounds down)
  best_divisor = 0;
  best_baud = 0;
  best_baud_diff = 0;
  for (i = 0; i < 2; i++) {
    int try_divisor = divisor + i;
    int baud_estimate;
    int baud_diff;

    // Round up to supported divisor value
    if (try_divisor <= 8) {
      // Round up to minimum supported divisor
      try_divisor = 8;
    } else if (ftdi->type != TYPE_AM && try_divisor < 12) {
      // BM doesn't support divisors 9 through 11 inclusive
      try_divisor = 12;
    } else if (divisor < 16) {
      // AM doesn't support divisors 9 through 15 inclusive
      try_divisor = 16;
    } else {
      if (ftdi->type == TYPE_AM) {
        // Round up to supported fraction (AM only)
        try_divisor += am_adjust_up[try_divisor & 7];
        if (try_divisor > 0x1FFF8) {
          // Round down to maximum supported divisor value (for AM)
          try_divisor = 0x1FFF8;
        }
      } else {
        if (try_divisor > 0x1FFFF) {
          // Round down to maximum supported divisor value (for BM)
          try_divisor = 0x1FFFF;
        }
      }
    }
    // Get estimated baud rate (to nearest integer)
    baud_estimate = (24000000 + (try_divisor / 2)) / try_divisor;
    // Get absolute difference from requested baud rate
    if (baud_estimate < baudrate) {
      baud_diff = baudrate - baud_estimate;
    } else {
      baud_diff = baud_estimate - baudrate;
    }
    if (i == 0 || baud_diff < best_baud_diff) {
      // Closest to requested baud rate so far
      best_divisor = try_divisor;
      best_baud = baud_estimate;
      best_baud_diff = baud_diff;
      if (baud_diff == 0) {
        // Spot on! No point trying
        break;
      }
    }
  }
  // Encode the best divisor value
  encoded_divisor = (best_divisor >> 3) | (frac_code[best_divisor & 7] << 14);
  // Deal with special cases for encoded value
  if (encoded_divisor == 1) {
    encoded_divisor = 0; // 3000000 baud
  } else if (encoded_divisor == 0x4001) {
    encoded_divisor = 1; // 2000000 baud (BM only)
  }
  // Split into "value" and "index" values
  *value = (unsigned short)(encoded_divisor & 0xFFFF);
  if (ftdi->type == TYPE_2232C || ftdi->type == TYPE_2232H ||
      ftdi->type == TYPE_4232H || ftdi->type == TYPE_232H) {
    *index = (unsigned short)(encoded_divisor >> 8);
    *index &= 0xFF00;
    *index |= ftdi->index;
  } else
    *index = (unsigned short)(encoded_divisor >> 16);

  // Return the nearest baud rate
  return best_baud;
}

/**
    Sets the chip baud rate

    \param ftdi pointer to ftdi_context
    \param baudrate baud rate to set

    \retval  0: all fine
    \retval -1: invalid baudrate
    \retval -2: setting baudrate failed
    \retval -3: USB device unavailable
*/
int ftdi_set_baudrate(struct ftdi_context *ftdi, int baudrate) {
  unsigned short value, index;
  int actual_baudrate;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-3, "USB device unavailable");

  if (ftdi->bitbang_enabled) {
    baudrate = baudrate * 4;
  }

  actual_baudrate = ftdi_convert_baudrate(baudrate, ftdi, &value, &index);
  if (actual_baudrate <= 0)
    ftdi_error_return(-1, "Silly baudrate <= 0.");

  // Check within tolerance (about 5%)
  if ((actual_baudrate * 2 < baudrate /* Catch overflows */) ||
      ((actual_baudrate < baudrate) ? (actual_baudrate * 21 < baudrate * 20)
                                    : (baudrate * 21 < actual_baudrate * 20)))
    ftdi_error_return(-1, "Unsupported baudrate. Note: bitbang baudrates are "
                          "automatically multiplied by 4");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_BAUDRATE_REQUEST, value, index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-2, "Setting new baudrate failed");

  ftdi->baudrate = baudrate;
  return 0;
}

/**
    Set (RS232) line characteristics.
    The break type can only be set via ftdi_set_line_property2()
    and defaults to "off".

    \param ftdi pointer to ftdi_context
    \param bits Number of bits
    \param sbit Number of stop bits
    \param parity Parity mode

    \retval  0: all fine
    \retval -1: Setting line property failed
*/
int ftdi_set_line_property(struct ftdi_context *ftdi, enum ftdi_bits_type bits,
                           enum ftdi_stopbits_type sbit,
                           enum ftdi_parity_type parity) {
  return ftdi_set_line_property2(ftdi, bits, sbit, parity, BREAK_OFF);
}

/**
    Set (RS232) line characteristics

    \param ftdi pointer to ftdi_context
    \param bits Number of bits
    \param sbit Number of stop bits
    \param parity Parity mode
    \param break_type Break type

    \retval  0: all fine
    \retval -1: Setting line property failed
    \retval -2: USB device unavailable
*/
int ftdi_set_line_property2(struct ftdi_context *ftdi, enum ftdi_bits_type bits,
                            enum ftdi_stopbits_type sbit,
                            enum ftdi_parity_type parity,
                            enum ftdi_break_type break_type) {
  unsigned short value = bits;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  switch (parity) {
  case NONE:
    value |= (0x00 << 8);
    break;
  case ODD:
    value |= (0x01 << 8);
    break;
  case EVEN:
    value |= (0x02 << 8);
    break;
  case MARK:
    value |= (0x03 << 8);
    break;
  case SPACE:
    value |= (0x04 << 8);
    break;
  }

  switch (sbit) {
  case STOP_BIT_1:
    value |= (0x00 << 11);
    break;
  case STOP_BIT_15:
    value |= (0x01 << 11);
    break;
  case STOP_BIT_2:
    value |= (0x02 << 11);
    break;
  }

  switch (break_type) {
  case BREAK_OFF:
    value |= (0x00 << 14);
    break;
  case BREAK_ON:
    value |= (0x01 << 14);
    break;
  }

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_DATA_REQUEST, value, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "Setting new line property failed");

  return 0;
}

/**
    Writes data in chunks (see ftdi_write_data_set_chunksize()) to the chip

    \param ftdi pointer to ftdi_context
    \param buf Buffer with the data
    \param size Size of the buffer

    \retval -666: USB device unavailable
    \retval <0: error code from usb_bulk_write()
    \retval >0: number of bytes written
*/
int ftdi_write_data(struct ftdi_context *ftdi, unsigned char *buf, int size) {
  int ret;
  int offset = 0;
  int total_written = 0;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-666, "USB device unavailable");

  while (offset < size) {
    int write_size = ftdi->writebuffer_chunksize;

    if (offset + write_size > size)
      write_size = size - offset;

    ret = usb_bulk_write(ftdi->usb_dev, ftdi->in_ep, (char *)buf + offset,
                         write_size, ftdi->usb_write_timeout);
    if (ret < 0)
      ftdi_error_return(ret, "usb bulk write failed");

    total_written += ret;
    offset += write_size;
  }

  return total_written;
}

#ifdef LIBFTDI_LINUX_ASYNC_MODE
#ifdef USB_CLASS_PTP
#error LIBFTDI_LINUX_ASYNC_MODE is not compatible with libusb-compat-0.1!
#endif
/* this is strongly dependent on libusb using the same struct layout. If libusb
   changes in some later version this may break horribly (this is for libusb
   0.1.12) */
struct usb_dev_handle {
  int fd;
  // some other stuff coming here we don't need
};

/**
    Check for pending async urbs
    \internal
*/
static int _usb_get_async_urbs_pending(struct ftdi_context *ftdi) {
  struct usbdevfs_urb *urb;
  int pending = 0;
  unsigned int i;

  for (i = 0; i < ftdi->async_usb_buffer_size; i++) {
    urb = &((struct usbdevfs_urb *)(ftdi->async_usb_buffer))[i];
    if (urb->usercontext != FTDI_URB_USERCONTEXT_COOKIE)
      pending++;
  }

  return pending;
}

/**
    Wait until one or more async URBs are completed by the kernel and mark their
    positions in the async-buffer as unused

    \param ftdi pointer to ftdi_context
    \param wait_for_more if != 0 wait for more than one write to complete
    \param timeout_msec max milliseconds to wait

    \internal
*/
static void _usb_async_cleanup(struct ftdi_context *ftdi, int wait_for_more,
                               int timeout_msec) {
  struct timeval tv;
  struct usbdevfs_urb *urb;
  int ret;
  fd_set writefds;
  int keep_going = 0;

  FD_ZERO(&writefds);
  FD_SET(ftdi->usb_dev->fd, &writefds);

  /* init timeout only once, select writes time left after call */
  tv.tv_sec = timeout_msec / 1000;
  tv.tv_usec = (timeout_msec % 1000) * 1000;

  do {
    ret = -1;
    urb = NULL;

    while (_usb_get_async_urbs_pending(ftdi) &&
           (ret = ioctl(ftdi->usb_dev->fd, USBDEVFS_REAPURBNDELAY, &urb)) ==
               -1 &&
           errno == EAGAIN) {
      if (keep_going && !wait_for_more) {
        /* don't wait if repeating only for keep_going */
        keep_going = 0;
        break;
      }

      /* wait for timeout msec or something written ready */
      select(ftdi->usb_dev->fd + 1, NULL, &writefds, NULL, &tv);
    }

    if (ret == 0 && urb != NULL) {
      /* got a free urb, mark it */
      urb->usercontext = FTDI_URB_USERCONTEXT_COOKIE;

      /* try to get more urbs that are ready now, but don't wait anymore */
      keep_going = 1;
    } else {
      /* no more urbs waiting */
      keep_going = 0;
    }
  } while (keep_going);
}

/**
    Wait until one or more async URBs are completed by the kernel and mark their
    positions in the async-buffer as unused.

    \param ftdi pointer to ftdi_context
    \param wait_for_more if != 0 wait for more than one write to complete (until
   write timeout)
*/
void ftdi_async_complete(struct ftdi_context *ftdi, int wait_for_more) {
  _usb_async_cleanup(ftdi, wait_for_more, ftdi->usb_write_timeout);
}

/**
    Stupid libusb does not offer async writes nor does it allow
    access to its fd - so we need some hacks here.
    \internal
*/
static int _usb_bulk_write_async(struct ftdi_context *ftdi, int ep, char *bytes,
                                 int size) {
  struct usbdevfs_urb *urb;
  int bytesdone = 0, requested;
  int ret, cleanup_count;
  unsigned int i;

  do {
    /* find a free urb buffer we can use */
    i = 0;
    urb = NULL;
    for (cleanup_count = 0; urb == NULL && cleanup_count <= 1;
         cleanup_count++) {
      if (i == ftdi->async_usb_buffer_size) {
        /* wait until some buffers are free */
        _usb_async_cleanup(ftdi, 0, ftdi->usb_write_timeout);
      }

      for (i = 0; i < ftdi->async_usb_buffer_size; i++) {
        urb = &((struct usbdevfs_urb *)(ftdi->async_usb_buffer))[i];
        if (urb->usercontext == FTDI_URB_USERCONTEXT_COOKIE)
          break; /* found a free urb position */
        urb = NULL;
      }
    }

    /* no free urb position found */
    if (urb == NULL)
      return -1;

    requested = size - bytesdone;
    if (requested > 4096)
      requested = 4096;

    memset(urb, 0, sizeof(urb));

    urb->type = USBDEVFS_URB_TYPE_BULK;
    urb->endpoint = ep;
    urb->flags = 0;
    urb->buffer = bytes + bytesdone;
    urb->buffer_length = requested;
    urb->signr = 0;
    urb->actual_length = 0;
    urb->number_of_packets = 0;
    urb->usercontext = 0;

    do {
      ret = ioctl(ftdi->usb_dev->fd, USBDEVFS_SUBMITURB, urb);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0)
      return ret; /* the caller can read errno to get more info */

    bytesdone += requested;
  } while (bytesdone < size);
  return bytesdone;
}

/**
    Writes data in chunks (see ftdi_write_data_set_chunksize()) to the chip.
    Does not wait for completion of the transfer nor does it make sure that
    the transfer was successful.

    This function could be extended to use signals and callbacks to inform the
    caller of completion or error - but this is not done yet, volunteers
   welcome.

    Works around libusb and directly accesses functions only available on Linux.
    Only available if compiled with --with-async-mode.

    \param ftdi pointer to ftdi_context
    \param buf Buffer with the data
    \param size Size of the buffer

    \retval -666: USB device unavailable
    \retval <0: error code from usb_bulk_write()
    \retval >0: number of bytes written
*/
int ftdi_write_data_async(struct ftdi_context *ftdi, unsigned char *buf,
                          int size) {
  int ret;
  int offset = 0;
  int total_written = 0;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-666, "USB device unavailable");

  while (offset < size) {
    int write_size = ftdi->writebuffer_chunksize;

    if (offset + write_size > size)
      write_size = size - offset;

    ret = _usb_bulk_write_async(ftdi, ftdi->in_ep, buf + offset, write_size);
    if (ret < 0)
      ftdi_error_return(ret, "usb bulk write async failed");

    total_written += ret;
    offset += write_size;
  }

  return total_written;
}
#endif // LIBFTDI_LINUX_ASYNC_MODE

/**
    Configure write buffer chunk size.
    Default is 4096.

    \param ftdi pointer to ftdi_context
    \param chunksize Chunk size

    \retval 0: all fine
    \retval -1: ftdi context invalid
*/
int ftdi_write_data_set_chunksize(struct ftdi_context *ftdi,
                                  unsigned int chunksize) {
  if (ftdi == NULL)
    ftdi_error_return(-1, "ftdi context invalid");

  ftdi->writebuffer_chunksize = chunksize;
  return 0;
}

/**
    Get write buffer chunk size.

    \param ftdi pointer to ftdi_context
    \param chunksize Pointer to store chunk size in

    \retval 0: all fine
    \retval -1: ftdi context invalid
*/
int ftdi_write_data_get_chunksize(struct ftdi_context *ftdi,
                                  unsigned int *chunksize) {
  if (ftdi == NULL)
    ftdi_error_return(-1, "ftdi context invalid");

  *chunksize = ftdi->writebuffer_chunksize;
  return 0;
}

/**
    Reads data in chunks (see ftdi_read_data_set_chunksize()) from the chip.

    Returns when at least one byte is available or when the latency timer has
   elapsed Automatically strips the two modem status bytes transfered during
   every read.

    \param ftdi pointer to ftdi_context
    \param buf Buffer to store data in
    \param size Size of the buffer

    \retval -666: USB device unavailable
    \retval <0: error code from usb_bulk_read()
    \retval  0: no data was available
    \retval >0: number of bytes read

*/
int ftdi_read_data(struct ftdi_context *ftdi, unsigned char *buf, int size) {
  int offset = 0, ret = 1, i, num_of_chunks, chunk_remains;
  int packet_size;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-666, "USB device unavailable");

  packet_size = ftdi->max_packet_size;
  // Packet size sanity check (avoid division by zero)
  if (packet_size == 0)
    ftdi_error_return(-1, "max_packet_size is bogus (zero)");

  // everything we want is still in the readbuffer?
  if (size <= ftdi->readbuffer_remaining) {
    memcpy(buf, ftdi->readbuffer + ftdi->readbuffer_offset, size);

    // Fix offsets
    ftdi->readbuffer_remaining -= size;
    ftdi->readbuffer_offset += size;

    /* printf("Returning bytes from buffer: %d - remaining: %d\n", size,
     * ftdi->readbuffer_remaining); */

    return size;
  }
  // something still in the readbuffer, but not enough to satisfy 'size'?
  if (ftdi->readbuffer_remaining != 0) {
    memcpy(buf, ftdi->readbuffer + ftdi->readbuffer_offset,
           ftdi->readbuffer_remaining);

    // Fix offset
    offset += ftdi->readbuffer_remaining;
  }
  // do the actual USB read
  while (offset < size && ret > 0) {
    ftdi->readbuffer_remaining = 0;
    ftdi->readbuffer_offset = 0;
    /* returns how much received */
    ret = usb_bulk_read(ftdi->usb_dev, ftdi->out_ep, (char *)ftdi->readbuffer,
                        ftdi->readbuffer_chunksize, ftdi->usb_read_timeout);
    if (ret < 0)
      ftdi_error_return(ret, "usb bulk read failed");

    if (ret > 2) {
      // skip FTDI status bytes.
      // Maybe stored in the future to enable modem use
      num_of_chunks = ret / packet_size;
      chunk_remains = ret % packet_size;
      // printf("ret = %X, num_of_chunks = %X, chunk_remains = %X,
      // readbuffer_offset = %X\n", ret, num_of_chunks, chunk_remains,
      // ftdi->readbuffer_offset);

      ftdi->readbuffer_offset += 2;
      ret -= 2;

      if (ret > packet_size - 2) {
        for (i = 1; i < num_of_chunks; i++)
          memmove(ftdi->readbuffer + ftdi->readbuffer_offset +
                      (packet_size - 2) * i,
                  ftdi->readbuffer + ftdi->readbuffer_offset + packet_size * i,
                  packet_size - 2);
        if (chunk_remains > 2) {
          memmove(ftdi->readbuffer + ftdi->readbuffer_offset +
                      (packet_size - 2) * i,
                  ftdi->readbuffer + ftdi->readbuffer_offset + packet_size * i,
                  chunk_remains - 2);
          ret -= 2 * num_of_chunks;
        } else
          ret -= 2 * (num_of_chunks - 1) + chunk_remains;
      }
    } else if (ret <= 2) {
      // no more data to read?
      return offset;
    }
    if (ret > 0) {
      // data still fits in buf?
      if (offset + ret <= size) {
        memcpy(buf + offset, ftdi->readbuffer + ftdi->readbuffer_offset, ret);
        // printf("buf[0] = %X, buf[1] = %X\n", buf[0], buf[1]);
        offset += ret;

        /* Did we read exactly the right amount of bytes? */
        if (offset == size)
          // printf("read_data exact rem %d offset %d\n",
          // ftdi->readbuffer_remaining, offset);
          return offset;
      } else {
        // only copy part of the data or size <= readbuffer_chunksize
        int part_size = size - offset;
        memcpy(buf + offset, ftdi->readbuffer + ftdi->readbuffer_offset,
               part_size);

        ftdi->readbuffer_offset += part_size;
        ftdi->readbuffer_remaining = ret - part_size;
        offset += part_size;

        /* printf("Returning part: %d - size: %d - offset: %d - ret: %d -
        remaining: %d\n", part_size, size, offset, ret,
        ftdi->readbuffer_remaining); */

        return offset;
      }
    }
  }
  // never reached
  return -127;
}

/**
    Configure read buffer chunk size.
    Default is 4096.

    Automatically reallocates the buffer.

    \param ftdi pointer to ftdi_context
    \param chunksize Chunk size

    \retval 0: all fine
    \retval -1: ftdi context invalid
*/
int ftdi_read_data_set_chunksize(struct ftdi_context *ftdi,
                                 unsigned int chunksize) {
  unsigned char *new_buf;

  if (ftdi == NULL)
    ftdi_error_return(-1, "ftdi context invalid");

  // Invalidate all remaining data
  ftdi->readbuffer_offset = 0;
  ftdi->readbuffer_remaining = 0;

  if ((new_buf = (unsigned char *)realloc(ftdi->readbuffer, chunksize)) == NULL)
    ftdi_error_return(-1, "out of memory for readbuffer");

  ftdi->readbuffer = new_buf;
  ftdi->readbuffer_chunksize = chunksize;

  return 0;
}

/**
    Get read buffer chunk size.

    \param ftdi pointer to ftdi_context
    \param chunksize Pointer to store chunk size in

    \retval 0: all fine
    \retval -1: FTDI context invalid
*/
int ftdi_read_data_get_chunksize(struct ftdi_context *ftdi,
                                 unsigned int *chunksize) {
  if (ftdi == NULL)
    ftdi_error_return(-1, "FTDI context invalid");

  *chunksize = ftdi->readbuffer_chunksize;
  return 0;
}

/**
    Enable bitbang mode.

    \deprecated use \ref ftdi_set_bitmode with mode BITMODE_BITBANG instead

    \param ftdi pointer to ftdi_context
    \param bitmask Bitmask to configure lines.
           HIGH/ON value configures a line as output.

    \retval  0: all fine
    \retval -1: can't enable bitbang mode
    \retval -2: USB device unavailable
*/
int ftdi_enable_bitbang(struct ftdi_context *ftdi, unsigned char bitmask) {
  unsigned short usb_val;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  usb_val = bitmask; // low byte: bitmask
  /* FT2232C: Set bitbang_mode to 2 to enable SPI */
  usb_val |= (ftdi->bitbang_mode << 8);

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_BITMODE_REQUEST, usb_val, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(
        -1, "unable to enter bitbang mode. Perhaps not a BM type chip?");

  ftdi->bitbang_enabled = 1;
  return 0;
}

/**
    Disable bitbang mode.

    \param ftdi pointer to ftdi_context

    \retval  0: all fine
    \retval -1: can't disable bitbang mode
    \retval -2: USB device unavailable
*/
int ftdi_disable_bitbang(struct ftdi_context *ftdi) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_BITMODE_REQUEST, 0, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(
        -1, "unable to leave bitbang mode. Perhaps not a BM type chip?");

  ftdi->bitbang_enabled = 0;
  return 0;
}

/**
    Enable/disable bitbang modes.

    \param ftdi pointer to ftdi_context
    \param bitmask Bitmask to configure lines.
           HIGH/ON value configures a line as output.
    \param mode Bitbang mode: use the values defined in \ref ftdi_mpsse_mode

    \retval  0: all fine
    \retval -1: can't enable bitbang mode
    \retval -2: USB device unavailable
*/
int ftdi_set_bitmode(struct ftdi_context *ftdi, unsigned char bitmask,
                     unsigned char mode) {
  unsigned short usb_val;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  usb_val = bitmask; // low byte: bitmask
  usb_val |= (mode << 8);
  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_BITMODE_REQUEST, usb_val, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "unable to configure bitbang mode. Perhaps selected "
                          "mode not supported on your chip?");

  ftdi->bitbang_mode = mode;
  ftdi->bitbang_enabled = (mode == BITMODE_RESET) ? 0 : 1;
  return 0;
}

/**
    Directly read pin state, circumventing the read buffer. Useful for bitbang
   mode.

    \param ftdi pointer to ftdi_context
    \param pins Pointer to store pins into

    \retval  0: all fine
    \retval -1: read pins failed
    \retval -2: USB device unavailable
*/
int ftdi_read_pins(struct ftdi_context *ftdi, unsigned char *pins) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE,
                      SIO_READ_PINS_REQUEST, 0, ftdi->index, (char *)pins, 1,
                      ftdi->usb_read_timeout) != 1)
    ftdi_error_return(-1, "read pins failed");

  return 0;
}

/**
    Set latency timer

    The FTDI chip keeps data in the internal buffer for a specific
    amount of time if the buffer is not full yet to decrease
    load on the usb bus.

    \param ftdi pointer to ftdi_context
    \param latency Value between 1 and 255

    \retval  0: all fine
    \retval -1: latency out of range
    \retval -2: unable to set latency timer
    \retval -3: USB device unavailable
*/
int ftdi_set_latency_timer(struct ftdi_context *ftdi, unsigned char latency) {
  unsigned short usb_val;

  if (latency < 1)
    ftdi_error_return(-1, "latency out of range. Only valid for 1-255");

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-3, "USB device unavailable");

  usb_val = latency;
  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_LATENCY_TIMER_REQUEST, usb_val, ftdi->index, NULL,
                      0, ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-2, "unable to set latency timer");

  return 0;
}

/**
    Get latency timer

    \param ftdi pointer to ftdi_context
    \param latency Pointer to store latency value in

    \retval  0: all fine
    \retval -1: unable to get latency timer
    \retval -2: USB device unavailable
*/
int ftdi_get_latency_timer(struct ftdi_context *ftdi, unsigned char *latency) {
  unsigned short usb_val;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE,
                      SIO_GET_LATENCY_TIMER_REQUEST, 0, ftdi->index,
                      (char *)&usb_val, 1, ftdi->usb_read_timeout) != 1)
    ftdi_error_return(-1, "reading latency timer failed");

  *latency = (unsigned char)usb_val;
  return 0;
}

/**
    Poll modem status information

    This function allows the retrieve the two status bytes of the device.
    The device sends these bytes also as a header for each read access
    where they are discarded by ftdi_read_data(). The chip generates
    the two stripped status bytes in the absence of data every 40 ms.

    Layout of the first byte:
    - B0..B3 - must be 0
    - B4       Clear to send (CTS)
                 0 = inactive
                 1 = active
    - B5       Data set ready (DTS)
                 0 = inactive
                 1 = active
    - B6       Ring indicator (RI)
                 0 = inactive
                 1 = active
    - B7       Receive line signal detect (RLSD)
                 0 = inactive
                 1 = active

    Layout of the second byte:
    - B0       Data ready (DR)
    - B1       Overrun error (OE)
    - B2       Parity error (PE)
    - B3       Framing error (FE)
    - B4       Break interrupt (BI)
    - B5       Transmitter holding register (THRE)
    - B6       Transmitter empty (TEMT)
    - B7       Error in RCVR FIFO

    \param ftdi pointer to ftdi_context
    \param status Pointer to store status information in. Must be two bytes.

    \retval  0: all fine
    \retval -1: unable to retrieve status information
    \retval -2: USB device unavailable
*/
int ftdi_poll_modem_status(struct ftdi_context *ftdi, unsigned short *status) {
  char usb_val[2];

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE,
                      SIO_POLL_MODEM_STATUS_REQUEST, 0, ftdi->index, usb_val, 2,
                      ftdi->usb_read_timeout) != 2)
    ftdi_error_return(-1, "getting modem status failed");

  *status = (usb_val[1] << 8) | (usb_val[0] & 0xFF);

  return 0;
}

/**
    Set flowcontrol for ftdi chip

    \param ftdi pointer to ftdi_context
    \param flowctrl flow control to use. should be
           SIO_DISABLE_FLOW_CTRL, SIO_RTS_CTS_HS, SIO_DTR_DSR_HS or
   SIO_XON_XOFF_HS

    \retval  0: all fine
    \retval -1: set flow control failed
    \retval -2: USB device unavailable
*/
int ftdi_setflowctrl(struct ftdi_context *ftdi, int flowctrl) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_FLOW_CTRL_REQUEST, 0, (flowctrl | ftdi->index),
                      NULL, 0, ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "set flow control failed");

  return 0;
}

/**
    Set dtr line

    \param ftdi pointer to ftdi_context
    \param state state to set line to (1 or 0)

    \retval  0: all fine
    \retval -1: set dtr failed
    \retval -2: USB device unavailable
*/
int ftdi_setdtr(struct ftdi_context *ftdi, int state) {
  unsigned short usb_val;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (state)
    usb_val = SIO_SET_DTR_HIGH;
  else
    usb_val = SIO_SET_DTR_LOW;

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_MODEM_CTRL_REQUEST, usb_val, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "set dtr failed");

  return 0;
}

/**
    Set rts line

    \param ftdi pointer to ftdi_context
    \param state state to set line to (1 or 0)

    \retval  0: all fine
    \retval -1: set rts failed
    \retval -2: USB device unavailable
*/
int ftdi_setrts(struct ftdi_context *ftdi, int state) {
  unsigned short usb_val;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (state)
    usb_val = SIO_SET_RTS_HIGH;
  else
    usb_val = SIO_SET_RTS_LOW;

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_MODEM_CTRL_REQUEST, usb_val, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "set of rts failed");

  return 0;
}

/**
    Set dtr and rts line in one pass

    \param ftdi pointer to ftdi_context
    \param dtr  DTR state to set line to (1 or 0)
    \param rts  RTS state to set line to (1 or 0)

    \retval  0: all fine
    \retval -1: set dtr/rts failed
    \retval -2: USB device unavailable
 */
int ftdi_setdtr_rts(struct ftdi_context *ftdi, int dtr, int rts) {
  unsigned short usb_val;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (dtr)
    usb_val = SIO_SET_DTR_HIGH;
  else
    usb_val = SIO_SET_DTR_LOW;

  if (rts)
    usb_val |= SIO_SET_RTS_HIGH;
  else
    usb_val |= SIO_SET_RTS_LOW;

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_MODEM_CTRL_REQUEST, usb_val, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "set of rts/dtr failed");

  return 0;
}

/**
    Set the special event character

    \param ftdi pointer to ftdi_context
    \param eventch Event character
    \param enable 0 to disable the event character, non-zero otherwise

    \retval  0: all fine
    \retval -1: unable to set event character
    \retval -2: USB device unavailable
*/
int ftdi_set_event_char(struct ftdi_context *ftdi, unsigned char eventch,
                        unsigned char enable) {
  unsigned short usb_val;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  usb_val = eventch;
  if (enable)
    usb_val |= 1 << 8;

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_EVENT_CHAR_REQUEST, usb_val, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "setting event character failed");

  return 0;
}

/**
    Set error character

    \param ftdi pointer to ftdi_context
    \param errorch Error character
    \param enable 0 to disable the error character, non-zero otherwise

    \retval  0: all fine
    \retval -1: unable to set error character
    \retval -2: USB device unavailable
*/
int ftdi_set_error_char(struct ftdi_context *ftdi, unsigned char errorch,
                        unsigned char enable) {
  unsigned short usb_val;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  usb_val = errorch;
  if (enable)
    usb_val |= 1 << 8;

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_SET_ERROR_CHAR_REQUEST, usb_val, ftdi->index, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "setting error character failed");

  return 0;
}

/**
   Set the eeprom size

   \param ftdi pointer to ftdi_context
   \param eeprom Pointer to ftdi_eeprom
   \param size

*/
void ftdi_eeprom_setsize(struct ftdi_context *ftdi, struct ftdi_eeprom *eeprom,
                         int size) {
  if (ftdi == NULL)
    return;

  ftdi->eeprom_size = size;
  eeprom->size = size;
}

/**
    Init eeprom with default values.

    \param eeprom Pointer to ftdi_eeprom
*/
void ftdi_eeprom_initdefaults(struct ftdi_eeprom *eeprom) {
  int i;

  if (eeprom == NULL)
    return;

  eeprom->vendor_id = 0x0403;
  eeprom->product_id = 0x6001;

  eeprom->self_powered = 1;
  eeprom->remote_wakeup = 1;
  eeprom->chip_type = TYPE_BM;

  eeprom->in_is_isochronous = 0;
  eeprom->out_is_isochronous = 0;
  eeprom->suspend_pull_downs = 0;

  eeprom->use_serial = 0;
  eeprom->change_usb_version = 0;
  eeprom->usb_version = 0x0200;
  eeprom->max_power = 0;

  eeprom->manufacturer = NULL;
  eeprom->product = NULL;
  eeprom->serial = NULL;
  for (i = 0; i < 5; i++) {
    eeprom->cbus_function[i] = 0;
  }
  eeprom->high_current = 0;
  eeprom->invert = 0;

  eeprom->size = FTDI_DEFAULT_EEPROM_SIZE;
}

/**
    Frees allocated memory in eeprom.

    \param eeprom Pointer to ftdi_eeprom
*/
void ftdi_eeprom_free(struct ftdi_eeprom *eeprom) {
  if (!eeprom)
    return;

  if (eeprom->manufacturer != 0) {
    free(eeprom->manufacturer);
    eeprom->manufacturer = 0;
  }
  if (eeprom->product != 0) {
    free(eeprom->product);
    eeprom->product = 0;
  }
  if (eeprom->serial != 0) {
    free(eeprom->serial);
    eeprom->serial = 0;
  }
}

/**
    Build binary output from ftdi_eeprom structure.
    Output is suitable for ftdi_write_eeprom().

    \note This function doesn't handle FT2232x devices. Only FT232x.
    \param eeprom Pointer to ftdi_eeprom
    \param output Buffer of 128 bytes to store eeprom image to

    \retval >0: free eeprom size
    \retval -1: eeprom size (128 bytes) exceeded by custom strings
    \retval -2: Invalid eeprom pointer
    \retval -3: Invalid cbus function setting
    \retval -4: Chip doesn't support invert
    \retval -5: Chip doesn't support high current drive
*/
int ftdi_eeprom_build(struct ftdi_eeprom *eeprom, unsigned char *output) {
  unsigned char i, j;
  unsigned short checksum, value;
  unsigned char manufacturer_size = 0, product_size = 0, serial_size = 0;
  int size_check;
  const int cbus_max[5] = {13, 13, 13, 13, 9};

  if (eeprom == NULL)
    return -2;

  if (eeprom->manufacturer != NULL)
    manufacturer_size = strlen(eeprom->manufacturer);
  if (eeprom->product != NULL)
    product_size = strlen(eeprom->product);
  if (eeprom->serial != NULL)
    serial_size = strlen(eeprom->serial);

  // highest allowed cbus value
  for (i = 0; i < 5; i++) {
    if ((eeprom->cbus_function[i] > cbus_max[i]) ||
        (eeprom->cbus_function[i] && eeprom->chip_type != TYPE_R))
      return -3;
  }
  if (eeprom->chip_type != TYPE_R) {
    if (eeprom->invert)
      return -4;
    if (eeprom->high_current)
      return -5;
  }

  size_check = eeprom->size;
  size_check -= 28; // 28 are always in use (fixed)

  // Top half of a 256byte eeprom is used just for strings and checksum
  // it seems that the FTDI chip will not read these strings from the lower half
  // Each string starts with two bytes; offset and type (0x03 for string)
  // the checksum needs two bytes, so without the string data that 8 bytes from
  // the top half
  if (eeprom->size >= 256)
    size_check = 120;
  size_check -= manufacturer_size * 2;
  size_check -= product_size * 2;
  size_check -= serial_size * 2;

  // eeprom size exceeded?
  if (size_check < 0)
    return (-1);

  // empty eeprom
  memset(output, 0, eeprom->size);

  // Addr 00: High current IO
  output[0x00] = eeprom->high_current ? HIGH_CURRENT_DRIVE : 0;
  // Addr 01: IN endpoint size (for R type devices, different for FT2232)
  if (eeprom->chip_type == TYPE_R) {
    output[0x01] = 0x40;
  }
  // Addr 02: Vendor ID
  output[0x02] = eeprom->vendor_id;
  output[0x03] = eeprom->vendor_id >> 8;

  // Addr 04: Product ID
  output[0x04] = eeprom->product_id;
  output[0x05] = eeprom->product_id >> 8;

  // Addr 06: Device release number (0400h for BM features)
  output[0x06] = 0x00;
  switch (eeprom->chip_type) {
  case TYPE_AM:
    output[0x07] = 0x02;
    break;
  case TYPE_BM:
    output[0x07] = 0x04;
    break;
  case TYPE_2232C:
    output[0x07] = 0x05;
    break;
  case TYPE_R:
    output[0x07] = 0x06;
    break;
  case TYPE_2232H:
    output[0x07] = 0x07;
    break;
  case TYPE_4232H:
    output[0x07] = 0x08;
    break;
  case TYPE_232H:
    output[0x07] = 0x09;
    break;
  default:
    output[0x07] = 0x00;
  }

  // Addr 08: Config descriptor
  // Bit 7: always 1
  // Bit 6: 1 if this device is self powered, 0 if bus powered
  // Bit 5: 1 if this device uses remote wakeup
  // Bit 4: 1 if this device is battery powered
  j = 0x80;
  if (eeprom->self_powered == 1)
    j |= 0x40;
  if (eeprom->remote_wakeup == 1)
    j |= 0x20;
  output[0x08] = j;

  // Addr 09: Max power consumption: max power = value * 2 mA
  output[0x09] = eeprom->max_power;

  // Addr 0A: Chip configuration
  // Bit 7: 0 - reserved
  // Bit 6: 0 - reserved
  // Bit 5: 0 - reserved
  // Bit 4: 1 - Change USB version
  // Bit 3: 1 - Use the serial number string
  // Bit 2: 1 - Enable suspend pull downs for lower power
  // Bit 1: 1 - Out EndPoint is Isochronous
  // Bit 0: 1 - In EndPoint is Isochronous
  //
  j = 0;
  if (eeprom->in_is_isochronous == 1)
    j = j | 1;
  if (eeprom->out_is_isochronous == 1)
    j = j | 2;
  if (eeprom->suspend_pull_downs == 1)
    j = j | 4;
  if (eeprom->use_serial == 1)
    j = j | 8;
  if (eeprom->change_usb_version == 1)
    j = j | 16;
  output[0x0A] = j;

  // Addr 0B: Invert data lines
  output[0x0B] = eeprom->invert & 0xff;

  // Addr 0C: USB version low byte when 0x0A bit 4 is set
  // Addr 0D: USB version high byte when 0x0A bit 4 is set
  if (eeprom->change_usb_version == 1) {
    output[0x0C] = eeprom->usb_version;
    output[0x0D] = eeprom->usb_version >> 8;
  }

  // Addr 0E: Offset of the manufacturer string + 0x80, calculated later
  // Addr 0F: Length of manufacturer string
  output[0x0F] = manufacturer_size * 2 + 2;

  // Addr 10: Offset of the product string + 0x80, calculated later
  // Addr 11: Length of product string
  output[0x11] = product_size * 2 + 2;

  // Addr 12: Offset of the serial string + 0x80, calculated later
  // Addr 13: Length of serial string
  output[0x13] = serial_size * 2 + 2;

  // Addr 14: CBUS function: CBUS0, CBUS1
  // Addr 15: CBUS function: CBUS2, CBUS3
  // Addr 16: CBUS function: CBUS5
  output[0x14] = eeprom->cbus_function[0] | (eeprom->cbus_function[1] << 4);
  output[0x15] = eeprom->cbus_function[2] | (eeprom->cbus_function[3] << 4);
  output[0x16] = eeprom->cbus_function[4];
  // Addr 17: Unknown

  // Dynamic content
  // In images produced by FTDI's FT_Prog for FT232R strings start at 0x18
  // Space till 0x18 should be considered as reserved.
  if (eeprom->chip_type >= TYPE_R) {
    i = 0x18;
  } else {
    i = 0x14;
  }
  if (eeprom->size >= 256)
    i = 0x80;

  // Output manufacturer
  output[0x0E] = i | 0x80; // calculate offset
  output[i++] = manufacturer_size * 2 + 2;
  output[i++] = 0x03; // type: string
  for (j = 0; j < manufacturer_size; j++) {
    output[i] = eeprom->manufacturer[j], i++;
    output[i] = 0x00, i++;
  }

  // Output product name
  output[0x10] = i | 0x80; // calculate offset
  output[i] = product_size * 2 + 2, i++;
  output[i] = 0x03, i++;
  for (j = 0; j < product_size; j++) {
    output[i] = eeprom->product[j], i++;
    output[i] = 0x00, i++;
  }

  // Output serial
  output[0x12] = i | 0x80; // calculate offset
  output[i] = serial_size * 2 + 2, i++;
  output[i] = 0x03, i++;
  for (j = 0; j < serial_size; j++) {
    output[i] = eeprom->serial[j], i++;
    output[i] = 0x00, i++;
  }

  // calculate checksum
  checksum = 0xAAAA;

  for (i = 0; i < eeprom->size / 2 - 1; i++) {
    value = output[i * 2];
    value += output[(i * 2) + 1] << 8;

    checksum = value ^ checksum;
    checksum = (checksum << 1) | (checksum >> 15);
  }

  output[eeprom->size - 2] = checksum;
  output[eeprom->size - 1] = checksum >> 8;

  return size_check;
}

/**
   Decode binary EEPROM image into an ftdi_eeprom structure.

   \param eeprom Pointer to ftdi_eeprom which will be filled in.
   \param buf Buffer of \a size bytes of raw eeprom data
   \param size size size of eeprom data in bytes

   \retval 0: all fine
   \retval -1: something went wrong

   FIXME: How to pass size? How to handle size field in ftdi_eeprom?
   FIXME: Strings are malloc'ed here and should be freed somewhere
*/
int ftdi_eeprom_decode(struct ftdi_eeprom *eeprom, unsigned char *buf,
                       int size) {
  unsigned char i, j;
  unsigned short checksum, eeprom_checksum, value;
  unsigned char manufacturer_size = 0, product_size = 0, serial_size = 0;
  int size_check;
  int eeprom_size = 128;

  if (eeprom == NULL)
    return -1;
#if 0
    size_check = eeprom->size;
    size_check -= 28; // 28 are always in use (fixed)

    // Top half of a 256byte eeprom is used just for strings and checksum
    // it seems that the FTDI chip will not read these strings from the lower half
    // Each string starts with two bytes; offset and type (0x03 for string)
    // the checksum needs two bytes, so without the string data that 8 bytes from the top half
    if (eeprom->size>=256)size_check = 120;
    size_check -= manufacturer_size*2;
    size_check -= product_size*2;
    size_check -= serial_size*2;

    // eeprom size exceeded?
    if (size_check < 0)
        return (-1);
#endif

  // empty eeprom struct
  memset(eeprom, 0, sizeof(struct ftdi_eeprom));

  // Addr 00: High current IO
  eeprom->high_current = (buf[0x02] & HIGH_CURRENT_DRIVE);

  // Addr 02: Vendor ID
  eeprom->vendor_id = buf[0x02] + (buf[0x03] << 8);

  // Addr 04: Product ID
  eeprom->product_id = buf[0x04] + (buf[0x05] << 8);

  value = buf[0x06] + (buf[0x07] << 8);
  switch (value) {
  case 0x0900:
    eeprom->chip_type = TYPE_232H;
    break;
  case 0x0800:
    eeprom->chip_type = TYPE_4232H;
    break;
  case 0x0700:
    eeprom->chip_type = TYPE_2232H;
    break;
  case 0x0600:
    eeprom->chip_type = TYPE_R;
    break;
  case 0x0400:
    eeprom->chip_type = TYPE_BM;
    break;
  case 0x0200:
    eeprom->chip_type = TYPE_AM;
    break;
  default: // Unknown device
    eeprom->chip_type = 0;
    break;
  }

  // Addr 08: Config descriptor
  // Bit 7: always 1
  // Bit 6: 1 if this device is self powered, 0 if bus powered
  // Bit 5: 1 if this device uses remote wakeup
  // Bit 4: 1 if this device is battery powered
  j = buf[0x08];
  if (j & 0x40)
    eeprom->self_powered = 1;
  if (j & 0x20)
    eeprom->remote_wakeup = 1;

  // Addr 09: Max power consumption: max power = value * 2 mA
  eeprom->max_power = buf[0x09];

  // Addr 0A: Chip configuration
  // Bit 7: 0 - reserved
  // Bit 6: 0 - reserved
  // Bit 5: 0 - reserved
  // Bit 4: 1 - Change USB version
  // Bit 3: 1 - Use the serial number string
  // Bit 2: 1 - Enable suspend pull downs for lower power
  // Bit 1: 1 - Out EndPoint is Isochronous
  // Bit 0: 1 - In EndPoint is Isochronous
  //
  j = buf[0x0A];
  if (j & 0x01)
    eeprom->in_is_isochronous = 1;
  if (j & 0x02)
    eeprom->out_is_isochronous = 1;
  if (j & 0x04)
    eeprom->suspend_pull_downs = 1;
  if (j & 0x08)
    eeprom->use_serial = 1;
  if (j & 0x10)
    eeprom->change_usb_version = 1;

  // Addr 0B: Invert data lines
  eeprom->invert = buf[0x0B];

  // Addr 0C: USB version low byte when 0x0A bit 4 is set
  // Addr 0D: USB version high byte when 0x0A bit 4 is set
  if (eeprom->change_usb_version == 1) {
    eeprom->usb_version = buf[0x0C] + (buf[0x0D] << 8);
  }

  // Addr 0E: Offset of the manufacturer string + 0x80, calculated later
  // Addr 0F: Length of manufacturer string
  manufacturer_size = buf[0x0F] / 2;
  if (manufacturer_size > 0)
    eeprom->manufacturer = (char *)malloc(manufacturer_size);
  else
    eeprom->manufacturer = NULL;

  // Addr 10: Offset of the product string + 0x80, calculated later
  // Addr 11: Length of product string
  product_size = buf[0x11] / 2;
  if (product_size > 0)
    eeprom->product = (char *)malloc(product_size);
  else
    eeprom->product = NULL;

  // Addr 12: Offset of the serial string + 0x80, calculated later
  // Addr 13: Length of serial string
  serial_size = buf[0x13] / 2;
  if (serial_size > 0)
    eeprom->serial = (char *)malloc(serial_size);
  else
    eeprom->serial = NULL;

  // Addr 14: CBUS function: CBUS0, CBUS1
  // Addr 15: CBUS function: CBUS2, CBUS3
  // Addr 16: CBUS function: CBUS5
  if (eeprom->chip_type == TYPE_R) {
    eeprom->cbus_function[0] = buf[0x14] & 0x0f;
    eeprom->cbus_function[1] = (buf[0x14] >> 4) & 0x0f;
    eeprom->cbus_function[2] = buf[0x15] & 0x0f;
    eeprom->cbus_function[3] = (buf[0x15] >> 4) & 0x0f;
    eeprom->cbus_function[4] = buf[0x16] & 0x0f;
  } else {
    for (j = 0; j < 5; j++)
      eeprom->cbus_function[j] = 0;
  }

  // Decode manufacturer
  i = buf[0x0E] & 0x7f; // offset
  for (j = 0; j < manufacturer_size - 1; j++) {
    eeprom->manufacturer[j] = buf[2 * j + i + 2];
  }
  eeprom->manufacturer[j] = '\0';

  // Decode product name
  i = buf[0x10] & 0x7f; // offset
  for (j = 0; j < product_size - 1; j++) {
    eeprom->product[j] = buf[2 * j + i + 2];
  }
  eeprom->product[j] = '\0';

  // Decode serial
  i = buf[0x12] & 0x7f; // offset
  for (j = 0; j < serial_size - 1; j++) {
    eeprom->serial[j] = buf[2 * j + i + 2];
  }
  eeprom->serial[j] = '\0';

  // verify checksum
  checksum = 0xAAAA;

  for (i = 0; i < eeprom_size / 2 - 1; i++) {
    value = buf[i * 2];
    value += buf[(i * 2) + 1] << 8;

    checksum = value ^ checksum;
    checksum = (checksum << 1) | (checksum >> 15);
  }

  eeprom_checksum = buf[eeprom_size - 2] + (buf[eeprom_size - 1] << 8);

  if (eeprom_checksum != checksum) {
    fprintf(stderr, "Checksum Error: %04x %04x\n", checksum, eeprom_checksum);
    return -1;
  }

  return 0;
}

/**
    Read eeprom location

    \param ftdi pointer to ftdi_context
    \param eeprom_addr Address of eeprom location to be read
    \param eeprom_val Pointer to store read eeprom location

    \retval  0: all fine
    \retval -1: read failed
    \retval -2: USB device unavailable
*/
int ftdi_read_eeprom_location(struct ftdi_context *ftdi, int eeprom_addr,
                              unsigned short *eeprom_val) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE,
                      SIO_READ_EEPROM_REQUEST, 0, eeprom_addr,
                      (char *)eeprom_val, 2, ftdi->usb_read_timeout) != 2)
    ftdi_error_return(-1, "reading eeprom failed");

  return 0;
}

/**
    Read eeprom

    \param ftdi pointer to ftdi_context
    \param eeprom Pointer to store eeprom into

    \retval  0: all fine
    \retval -1: read failed
    \retval -2: USB device unavailable
*/
int ftdi_read_eeprom(struct ftdi_context *ftdi, unsigned char *eeprom) {
  int i;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  for (i = 0; i < ftdi->eeprom_size / 2; i++) {
    if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE,
                        SIO_READ_EEPROM_REQUEST, 0, i, (char *)eeprom + (i * 2),
                        2, ftdi->usb_read_timeout) != 2)
      ftdi_error_return(-1, "reading eeprom failed");
  }

  return 0;
}

/*
    ftdi_read_chipid_shift does the bitshift operation needed for the
   FTDIChip-ID Function is only used internally \internal
*/
static unsigned char ftdi_read_chipid_shift(unsigned char value) {
  return ((value & 1) << 1) | ((value & 2) << 5) | ((value & 4) >> 2) |
         ((value & 8) << 4) | ((value & 16) >> 1) | ((value & 32) >> 1) |
         ((value & 64) >> 4) | ((value & 128) >> 2);
}

/**
    Read the FTDIChip-ID from R-type devices

    \param ftdi pointer to ftdi_context
    \param chipid Pointer to store FTDIChip-ID

    \retval  0: all fine
    \retval -1: read failed
    \retval -2: USB device unavailable
*/
int ftdi_read_chipid(struct ftdi_context *ftdi, unsigned int *chipid) {
  unsigned int a = 0, b = 0;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE,
                      SIO_READ_EEPROM_REQUEST, 0, 0x43, (char *)&a, 2,
                      ftdi->usb_read_timeout) == 2) {
    a = a << 8 | a >> 8;
    if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE,
                        SIO_READ_EEPROM_REQUEST, 0, 0x44, (char *)&b, 2,
                        ftdi->usb_read_timeout) == 2) {
      b = b << 8 | b >> 8;
      a = (a << 16) | (b & 0xFFFF);
      a = ftdi_read_chipid_shift(a) | ftdi_read_chipid_shift(a >> 8) << 8 |
          ftdi_read_chipid_shift(a >> 16) << 16 |
          ftdi_read_chipid_shift(a >> 24) << 24;
      *chipid = a ^ 0xa5f0f7d1;
      return 0;
    }
  }

  ftdi_error_return(-1, "read of FTDIChip-ID failed");
}

/**
    Guesses size of eeprom by reading eeprom and comparing halves - will not
   work with blank eeprom Call this function then do a write then call again to
   see if size changes, if so write again.

    \param ftdi pointer to ftdi_context
    \param eeprom Pointer to store eeprom into
    \param maxsize the size of the buffer to read into

    \retval -1: eeprom read failed
    \retval -2: USB device unavailable
    \retval >=0: size of eeprom
*/
int ftdi_read_eeprom_getsize(struct ftdi_context *ftdi, unsigned char *eeprom,
                             int maxsize) {
  int i = 0, j, minsize = 32;
  int size = minsize;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  do {
    for (j = 0; i < maxsize / 2 && j < size; j++) {
      if (usb_control_msg(
              ftdi->usb_dev, FTDI_DEVICE_IN_REQTYPE, SIO_READ_EEPROM_REQUEST, 0,
              i, (char *)eeprom + (i * 2), 2, ftdi->usb_read_timeout) != 2)
        ftdi_error_return(-1, "eeprom read failed");
      i++;
    }
    size *= 2;
  } while (size <= maxsize && memcmp(eeprom, &eeprom[size / 2], size / 2) != 0);

  return size / 2;
}

/**
    Write eeprom location

    \param ftdi pointer to ftdi_context
    \param eeprom_addr Address of eeprom location to be written
    \param eeprom_val Value to be written

    \retval  0: all fine
    \retval -1: read failed
    \retval -2: USB device unavailable
*/
int ftdi_write_eeprom_location(struct ftdi_context *ftdi, int eeprom_addr,
                               unsigned short eeprom_val) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_WRITE_EEPROM_REQUEST, eeprom_val, eeprom_addr, NULL,
                      0, ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "unable to write eeprom");

  return 0;
}

/**
    Write eeprom

    \param ftdi pointer to ftdi_context
    \param eeprom Pointer to read eeprom from

    \retval  0: all fine
    \retval -1: read failed
    \retval -2: USB device unavailable
*/
int ftdi_write_eeprom(struct ftdi_context *ftdi, unsigned char *eeprom) {
  unsigned short usb_val, status;
  int i, ret;

  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  /* These commands were traced while running MProg */
  if ((ret = ftdi_usb_reset(ftdi)) != 0)
    return ret;
  if ((ret = ftdi_poll_modem_status(ftdi, &status)) != 0)
    return ret;
  if ((ret = ftdi_set_latency_timer(ftdi, 0x77)) != 0)
    return ret;

  for (i = 0; i < ftdi->eeprom_size / 2; i++) {
    usb_val = eeprom[i * 2];
    usb_val += eeprom[(i * 2) + 1] << 8;
    if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                        SIO_WRITE_EEPROM_REQUEST, usb_val, i, NULL, 0,
                        ftdi->usb_write_timeout) != 0)
      ftdi_error_return(-1, "unable to write eeprom");
  }

  return 0;
}

/**
    Erase eeprom

    This is not supported on FT232R/FT245R according to the MProg manual from
   FTDI.

    \param ftdi pointer to ftdi_context

    \retval  0: all fine
    \retval -1: erase failed
    \retval -2: USB device unavailable
*/
int ftdi_erase_eeprom(struct ftdi_context *ftdi) {
  if (ftdi == NULL || ftdi->usb_dev == NULL)
    ftdi_error_return(-2, "USB device unavailable");

  if (usb_control_msg(ftdi->usb_dev, FTDI_DEVICE_OUT_REQTYPE,
                      SIO_ERASE_EEPROM_REQUEST, 0, 0, NULL, 0,
                      ftdi->usb_write_timeout) != 0)
    ftdi_error_return(-1, "unable to erase eeprom");

  return 0;
}

/**
    Get string representation for last error code

    \param ftdi pointer to ftdi_context

    \retval Pointer to error string
*/
const char *ftdi_get_error_string(struct ftdi_context *ftdi) {
  if (ftdi == NULL)
    return "";

  return ftdi->error_str;
}

/* @} end of doxygen libftdi group */

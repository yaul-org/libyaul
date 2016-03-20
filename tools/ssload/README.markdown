### About

`ssload` is (hopefully) a be-all tool to communicate with your Sega Saturn

### Requirements

 - FTDI USB driver (`libftdi`) (`HAVE_LIBFTD2XX=0`)
 - D2XX Direct Drivers (`libftd2xx`) (`HAVE_LIBFTD2XX=1`)

### Installation

  If all requirements are met, as a _normal_ user and under the root
  of the `tools/ssload` directory, perform the following to build the
  assembler

    make

  In order to use `ssload` as a normal user, under Linux, write your
  own udev rule in `/etc/udev/rules.d/`

    ACTION=="add", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="0403", ATTR{idProduct}=="6001", \
        NAME="bus/usb/$env{BUSNUM}/$env{DEVNUM}", GROUP="dialout", MODE="0664"

### Usage (if using D2XX Direct Drivers library)

  If under Linux, be sure to remove modules `ftdi_sio` and `usbserial`

    sudo rmmod ftdi_sio
    sudo rmmod usbserial

### Errors

  Found a bug or do you have a suggestion to make `ssload` even
  better than it already is? Create an issue [here][1].

[1]: https://github.com/mrkotfw/libyaul/issues

### About

`ssload` is (hopefully) a be-all tool to communicate with your Sega Saturn.

### Dependencies

 - FTDI USB driver (`libftdi`) (`HAVE_LIBFTD2XX=0`)
 - D2XX Direct Drivers (`libftd2xx`) (`HAVE_LIBFTD2XX=1`)

### Installation

  If all requirements are met, as a _normal_ user and under the root of the
  `tools/ssload` directory, perform the following to build the assembler

    make

  In order to use `ssload` as a normal user, under Linux, write your own udev
  rule in `/etc/udev/rules.d/50-udev-default.rules`

    SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE="0666"

#### Notes about MinGW

  Define `HAVE_LIBUSB_WIN32` in order to compile under MinGW64.

#### Notes about Linux

  If under Linux and D2XX Direct Drivers library is used, be sure to remove
  modules `ftdi_sio` and `usbserial`.

    sudo rmmod ftdi_sio
    sudo rmmod usbserial

### Errors

  Found a bug or do you have a suggestion to make `ssload` even better than it
  already is? Create an issue [here][1].

[1]: https://github.com/ijacquez/libyaul/issues

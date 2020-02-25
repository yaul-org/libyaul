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

  It doesn't work unless `libftdi-0.20` is used. Building all versions of
  `libftdi1` yields issues.

  Below are the steps used to build `libftdi1`. Note, the
  `mingw32/mingw-w64-i686-cmake` package is required.

    cmake \
        -DCMAKE_SYSTEM_NAME=Windows \
        -DCMAKE_FIND_ROOT_PATH=c:/path/to/msys64/mingw32 \
        -DLIBUSB_LIBRARIES=c:/path/to/msys64/mingw32/lib/libusb-1.0.dll.a \
        -DLIBUSB_INCLUDE_DIR=c:/path/to/msys64/mingw32/include/libusb-1.0 \
        -DCMAKE_C_COMPILER=/mingw32/bin/gcc.exe \
        -DCMAKE_CXX_COMPILER=/mingw32/bin/g++.exe -G "Unix Makefiles" ..

#### Notes about Linux

  If under Linux and D2XX Direct Drivers library is used, be sure to remove
  modules `ftdi_sio` and `usbserial`.

    sudo rmmod ftdi_sio
    sudo rmmod usbserial

### Errors

  Found a bug or do you have a suggestion to make `ssload` even better than it
  already is? Create an issue [here][1].

[1]: https://github.com/ijacquez/libyaul/issues

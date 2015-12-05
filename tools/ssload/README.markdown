### About

`ssload` is (hopefully) a be-all tool to communicate with your Sega Saturn

### Requirements

 - D2XX Direct Drivers (`libftd2xx`)

### Installation

  If all requirements are met, as a _normal_ user and under the root
  of the `tools/ssload` directory, perform the following to build the
  assembler

    make

### Usage

  If under Linux, be sure to remove modules `ftdi_sio` and `usbserial`

    sudo rmmod ftdi_sio
    sudo rmmod usbserial

### Errors

  Found a bug or do you have a suggestion to make `ssload` even
  better than it already is? Create an issue [here][1].

[1]: https://github.com/mrkotfw/libyaul/issues

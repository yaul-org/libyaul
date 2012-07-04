About
=====

Thorough examples (and tests) using different features of `libyaul`

Usage
=====

### Requirements
 - GCC 4.6.3 or greater SH7604 cross-compiler tool-chain (see `tools/build-scripts/`)

### CD image creation

To build an ISO, execute the following

    cd example/
    cp ../common/bootstrap/ip.S .
    make image

The following files will be built
 - `example.iso` - The ISO image ready to be burned onto a CD-R
 - `example.bin` - The executable

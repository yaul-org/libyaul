Description
===========

This example initializes the DRAM cartridge, loads in a ROMDISK file system onto the cart's memory. From there, `/hello.world` is loaded and read into a buffer.

### Building the ROMDISK file system

Before building, you must have [`genromfs`][1] installed. Perform the following to build the file system

    genromfs -f root.romdisk -d romdisk/ -V "ROOT"

[1]: http://packages.debian.org/stable/genromfs

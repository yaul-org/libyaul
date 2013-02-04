Description
===========

This example initializes the GDB stub which allows you to debug a program running on a machine that cannot run `gdb` in the usual way.

## How to use

Have the program `program.bin` be already running on the Sega Saturn. Because `gdb_init()` has been invoked, the program will wait until a successful connection has been established.

### Starting the GDB proxy

Execute `gdb-ftdi-proxy` from `tools/gdb-ftdi-proxy`:

    ./gdb-ftdi-proxy 1234

### Connecting to the Saturn (remote target)

On the `gdb` host machine (the machine that built the program), you will need an unstripped copy of your program, since `gdb` needs symbol and debugging information. Start up `gdb` as usual, using the name of the local copy of your program as the first argument (file extension ends with `.elf`.

    sh-elf-gdb program.elf

Once the connection has been established, you can use all the usual commands to examine and change data. The remote program is already running; you can use `step` and `continue`, and you do not need to use run.

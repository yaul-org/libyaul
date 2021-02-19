Yet Another Useless [Saturn] Library
===
<p align="center">
  <img width="700" height="630" src=".images/logo.png">
</p>

[![Build status](https://img.shields.io/travis/ijacquez/libyaul.svg)](https://travis-ci.org/ijacquez/libyaul) [![Issues](https://img.shields.io/github/issues/ijacquez/libyaul.svg)](https://github.com/ijacquez/libyaul/issues) [![Last commit](https://img.shields.io/github/last-commit/ijacquez/libyaul.svg)](https://github.com/ijacquez/libyaul/commits/develop) ![Size](https://img.shields.io/github/repo-size/ijacquez/libyaul.svg) [![Join us on Discord](https://img.shields.io/discord/531844227655532554.svg)]( https://discord.gg/S434dWA)

Yaul is an open source development kit for the SEGA Saturn. The SDK as a whole
aims to minimize the _painful_ experience that is developing for the Saturn by
providing lightweight abstractions between your program and the hardware.

## Pre-installation requirements

### Windows

Download the _Yaul MSYS2 64-bit_ installer from the release [page][2].

Once _Yaul MSYS2 64-bit_ is installed, your entire environment is set up!
Examples can be found in `/opt/libyaul/examples`.

### Linux

Verify that the following packages are installed.

 - Via _Pacman_:

       pacman -S \
       git make gcc wget unzip zip p7zip diffutils dos2unix patch xorriso

 - Via _APT_ (Debian based Linux distributions):

       apt-get install \
       git make gcc wget unzip zip p7zip diffutils dos2unix patch xorriso

### MacOS X

Verify that the following packages are installed (via _Brew_):

    brew install make gcc wget unzip zip p7zip diffutils dos2unix gpatch xorriso bash

Please note that a **case-sensitive** file system is required.

## Installing the tool-chain

There are two options, downloading a prebuilt tool-chain, or building from
source.

### Download pre-built

A prebuilt tool-chain can be downloaded from the table below. Create a directory
`tool-chain` and extract the contents of the archive into it.

If you're on Windows, and you've downloaded _Yaul MSYS2 64-bit_, the tool-chain
is already installed in `/opt/x-tools`.

| Platform        | Architecture | Tool-chain link |
|-----------------|--------------|-----------------|
| Linux           | x86          | [Available][3]  |
| Linux           | x86_64       | [Available][3]  |
| Windows (MinGW) | x86_64       | [Available][3]  |
| MacOS X         | x86_64       | Not available   |

### M68k

_Currently unavailable._

### Build from source

If building from source, follow the instructions found in the `build-scripts`
directory. Take note of the absolute path to the tool-chain.

## Setting up environment file

1. Copy the template `yaul.env.in` to your home directory as `.yaul.env`. This
   is your environment file.

2. Open `.yaul.env` in a text editor and change the following to define your
   environment:

   1. Set the absolute path to the tool-chain in `YAUL_INSTALL_ROOT`.
   2. If necessary, set `YAUL_PROG_SH_PREFIX` and `YAUL_ARCH_SH_PREFIX`.
   3. Set the absolute path to where the `libyaul` source tree is located in
      `YAUL_BUILD_ROOT`.
   4. Set the type of development cart you own in `YAUL_OPTION_DEV_CARTRIDGE`.
      If none, set to 0 (zero).
   5. Set whether to system reset or spin on abort() in
      `YAUL_OPTION_SPIN_ON_ABORT`. To system reset, set to 0 (zero).
   6. Enable RTags/Irony/ccls support by setting `YAUL_CDB` to 1. To disable,
      set to 0 (zero).

   Setting the wrong values may result in compilation errors.

3. Read the environment file `.yaul.env` into your current shell.

       $ source ~/.yaul.env

4. Reading the environment file needs to be done every time a new shell is
   opened. To avoid having to do this every time, add the line below to your
   shell's startup file.

       $ echo 'source ~/.yaul.env' >> ~/.bash_profile

   If `.bash_profile` is not used, use `.profile` instead. This is dependent on
   your set up.

## Building Yaul

1. Initialize and update all submodules.

       $ git submodule init
       $ git submodule update -f --recursive

2. Build and install the supported libraries.

       $ SILENT=1 make install-release

   If any given library is being debugged, use the `install-debug` target
   instead. Either _release_ or _debug_ can currently be installed at one
   time. It's possible to switch between the two in the same installation.

   To find more about other targets, call `make list-targets`.

3. Build and install the tools.

       $ SILENT=1 make install-tools

## Building and running an example

You can now build any of the given examples in the [`libyaul-examples`][1]
submodule.

## Documentation

Visit [yaul.org][4].

## Contact

You can find me (*@mrkotfw*) on [Discord]( https://discord.gg/S434dWA).

[1]: https://github.com/ijacquez/libyaul-examples
[2]: https://github.com/ijacquez/libyaul-installer/releases
[3]: github.com/ijacquez/libyaul-packages/releases/latest
[4]: https://yaul.org/

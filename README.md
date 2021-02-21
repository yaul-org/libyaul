Yet Another Useless [Saturn] Library
===
<div align="center">
  <img src=".images/logo.png" width="50%" height="50%"/>
</div>
<div align="center">
  <a href="https://travis-ci.org/ijacquez/libyaul">
    <img src="https://img.shields.io/travis/ijacquez/libyaul.svg" alt="Build status"/>
  </a>
  <a href="https://github.com/ijacquez/libyaul/issues">
    <img src="https://img.shields.io/github/issues/ijacquez/libyaul.svg" alt="Issues"/>
  </a>
  <a href="https://github.com/ijacquez/libyaul/commits/develop">
    <img src="https://img.shields.io/github/last-commit/ijacquez/libyaul.svg" alt="Last commit"/>
  </a>
  <a href="https://discord.gg/S434dWA">
    <img src="https://discord.gg/S434dWA" alt="Join us on Discord"/>
  </a>
</div>

Yaul is an open source development kit for the SEGA Saturn. The SDK as a whole
aims to minimize the _painful_ experience that is developing for the Saturn by
providing lightweight abstractions between your program and the hardware.

## Documentation

Visit [yaul.org][1].

## Pre-built Installation

### Windows

#### Installer

If you don't already have MSYS2 installed, download the _Yaul MSYS2 64-bit_
installer from the release [page][2].

#### Existing MSYS2

If you already have MSYS2 installed, follow the directions below to setup access
to the package repository.

_Instructions coming soon!_

### Linux

#### Arch

Follow the directions below to setup access to the Arch Linux package
repository, or build the [packages][ yourself.

_Instructions coming soon!_

#### Debian based

There are currently no `.deb` packages available.

#### Other

A pre-built tool-chain can be downloaded from the table below. Create a
directory `tool-chains` and extract the contents of the archive into it.

| Platform        | Architecture | Tool-chain link |
|-----------------|--------------|-----------------|
| Linux           | x86          | [Available][3]  |
| Linux           | x86_64       | [Available][3]  |

### MacOS X

There are currently no packages available.

## Building tool-chain from source

Follow the instructions found in the [`build-scripts/`][5] submodule directory.
Take note of the absolute path to the tool-chain.

## Building Yaul

### Cloning the repository

Clone the repository

      git clone --recursive "https://github.com/ijacquez/libyaul.git"

### Setting up environment file

1. Copy the template `yaul.env.in` to your home directory as `.yaul.env`. This
   is your environment file.

2. Open `.yaul.env` in a text editor and change the following to define your
   environment:

   1. Set the absolute path to the tool-chain in `YAUL_INSTALL_ROOT`.
   2. If necessary, set `YAUL_PROG_SH_PREFIX` and `YAUL_ARCH_SH_PREFIX`.
   3. Set the absolute path to where the `libyaul` source tree is located in
      `YAUL_BUILD_ROOT`.
   4. Set the type of development cart you own in `YAUL_OPTION_DEV_CARTRIDGE`.
      If you don't own a development cart, or you will only test on emulators,
      set to 0 (zero).
   5. Enable RTags/Irony/ccls support by setting `YAUL_CDB` to 1. To disable,
      set to 0 (zero).

   Setting the wrong values may result in compilation errors.

3. Read the environment file `.yaul.env` into your current shell.

       source ~/.yaul.env

4. Reading the environment file needs to be done every time a new shell is
   opened. To avoid having to do this every time, add the line below to your
   shell's startup file.

       echo 'source ~/.yaul.env' >> ~/.bash_profile

   If `.bash_profile` is not used, use `.profile` instead. This is dependent on
   your set up.

### Building

2. Build and install the supported libraries.

       SILENT=1 make install-release

   If any given library in Yaul is being debugged, use the `install-debug`
   target instead. Either _release_ or _debug_ can currently be installed at one
   time. It's possible to switch between the two in the same installation.

   To find more about other targets, call `make list-targets`.

3. Build and install the tools.

       SILENT=1 make install-tools

### Building and running an example

You can now build any of the given examples in the [`examples`][4] submodule
directory.

## Contact

You can find me (*@mrkotfw*) on [Discord]( https://discord.gg/S434dWA).

[1]: https://yaul.org/
[2]: https://github.com/ijacquez/libyaul-installer/releases/latest
[3]: https://github.com/ijacquez/libyaul-packages/releases/latest
[4]: https://github.com/ijacquez/libyaul-examples
[5]: https://github.com/ijacquez/libyaul-build-scripts

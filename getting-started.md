---
layout: page
title: Getting Started
permalink: /getting-started/
---

## Pre-installation requirements

### Windows
If MSYS2 is not installed on your system, download the installer from the [MSYS2][4] web page and follow the instructions [here][5].

Once MSYS2 is installed, install the following packages via the pacman package manager:

    pacman -S \
    git make gcc wget unzip zip p7zip diffutils dos2unix patch tar

### Linux

Verify that the following packages are installed.

 - Via _Pacman_: 

       pacman -S \
       git make gcc wget unzip zip p7zip diffutils dos2unix patch

 - Via _APT_:

       apt-get install \
       git make gcc wget unzip zip p7zip diffutils dos2unix patch

### OS X

Verify that the following packages are installed (via _Brew_):

    brew install make gcc wget unzip zip p7zip diffutils dos2unix gpatch 

## Installing the tool-chain

There are two options, downloading a prebuilt tool-chain, or building from source.

### Download pre-built

A prebuilt tool-chain can be downloaded in [Downloads][2]. Create a directory `tool-chain` and extract the contents of the archive into it. For Windows users, the `tool-chain` must reside under the MSYS2 system root path (i.e. `C:\msys64`). Take note of the absolute path to the tool-chain.

### Build from source

If building from source, follow the instructions found in the `build-scripts` directory. Take note of the absolute path to the tool-chain.

## Building Yaul

3. Initialize and update all submodules.

       $ git submodule init
       $ git submodule update -f --recursive

4. Copy the template `yaul.env.in` to your home directory as
   `.yaul.env`. This is your environment file.

5. Open `.yaul.env` in a text editor and change the following to
   define your environment:

   1. Set the absolute path to the tool-chain in `INSTALL_ROOT`.
   2. Set the absolute path to where the `libyaul` source tree is
      located in `BUILD_ROOT`.
   3. Set the type of development cart you own in
      `OPTION_DEV_CARTRIDGE`. If none, set to 0 (zero).

   Setting the wrong values may result in compilation errors.

6. Read the environment file `.yaul.env` into your current shell.

       $ source ~/.yaul.env

7. Reading the environment file needs to be done every time a new
   shell is opened. To avoid having to do this every time, add the
   line below to your shell's startup file.

       $ echo 'source ~/.yaul.env' >> ~/.bash_profile

   If `.bash_profile` is not used, use `.profile` instead. This is
   dependent on your set up.

8. Build and install the supported libraries.

       $ SILENT=1 make install-release

   If any given library is being debugged, use the `install-debug`
   target instead. Either _release_ or _debug_ can currently be
   installed at one time. It's possible to switch between the two in
   the same installation.

   To find more about other targets, call `make list-targets`.

9. Build and install the tools.

       $ SILENT=1 make install-tools

## Building and running an example

10. You can now build any of the given examples in the
    `libyaul-examples` submodule.

[2]: /downloads
[3]: https://freenode.net/
[4]: https://www.msys2.org/
[5]: https://github.com/msys2/msys2/wiki/MSYS2-installation

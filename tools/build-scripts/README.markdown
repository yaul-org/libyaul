Usage
=====

### Requirements
 - POSIX Bourne shell
 - Software dependencies:
  - GMP
  - MPFR
  - MPC
  - Texinfo
  - ncurses
 - Time and especially patience

### Installation
  If all requirements are met, as a _normal_ user and under the root of the `tools/build-scripts` directory, perform the following to build the cross compiler

    cp -v config{.in,}

In the `config` you just copied, change the appropriate fields.

    emacs config

If you haven't already installed the dependencies:

    # Debian/Ubuntu
    sudo apt-get install libgmp3-dev libmpfr-dev libmpc-dev libncurses5-dev texinfo

    # Fedora/CentOS
    sudo yum install gmp-devel mpfr-devel libmpc-devel ncurses-devel texi2html texinfo

Before you begin building the tool-chain, be sure to double-check your changes in `config`. Otherwise,

    ./build-compiler

After a successful build, be sure to add the following paths to `$PATH`

    PATH="${BUILD_INSTALL_DIR}/sh-elf/bin:${BUILD_INSTALL_DIR}/m68k-elf/bin:${PATH}"
    export PATH

### Errors

   Be sure to check out the log files in the `${BUILD_SRC_DIR}/` directory.

<p align="center">
<img src="https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000" alt="license">
  <a href="https://travis-ci.org/ijacquez/libyaul">
    <img src="https://travis-ci.org/ijacquez/libyaul.svg?branch=master" alt="build status">
  </a>
</p>

Yaul
====
  Yaul is an open source SEGA Saturn development kit.

Installation
============
  1. Build the tool-chain (see `build-scripts/`) first. Take note where the tool-chain is installed. Once installed, under the root of the `libyaul` source tree, build and install the library.

    cd libyaul

  2. Initialize and update all submodules.

    git submodule init
    git submodule update -f --recursive

  3. Copy the template `yaul.env.in` and rename it to `yaul.env`.

  4. Change the necessary parameters.
     1. Set the absolute path to the tool-chain in `INSTALL_ROOT`.
     2. Set the absolute path to where the `libyaul` source tree is located in `BUILD_ROOT`.
     3. Set the type of development cart you own in `OPTION_DEV_CARTRIDGE`. If none, set to 0.

  5. Read the file `yaul.env` into your current shell. Note, this needs to be done every time a new shell is opened.

    . yaul.env

  6. Build and install the supported libraries.

    make install-release

  7. Build and install the tools.

    make install-tools

  8. Once the library and tools has been installed, you can now build any of the given examples in the `libyaul-examples` submodule.

Contributing
============

Want to contribute? Great! I could always use some extra help! Found a bug or do you have a suggestion? If so, I'd love to hear it. Create an issue [here][1].

[1]: https://github.com/ijacquez/libyaul/issues

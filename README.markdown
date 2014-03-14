About
=====
  `libyaul` is an awesome open source SEGA Saturn software development kit that provides flexible low-level routines that are easily able to be composed to produce higher-level abstractions to further ease the burden of developing applications. For actual demonstrations running on real hardware, see my [YouTube][2] channel.

### Authors
 * Israel Jacquez

 For others, see the `CONTRIBUTORS` file.

### License
  To see the full text of the license, see the `LICENSE` file.

Usage
=====

### Requirements
 - CMake ver. 2.8 or greater (a Cross-Platform Makefile Generator)
 - GCC 4.6.3 or greater SH7604 cross-compiler tool-chain (see `tools/build-scripts/`)

### Installation
  If all requirements are met, as a _normal_ user and under the root of the `libyaul` source directory, perform the following to build the library

    mkdir -p build
    cd build/
    cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../Sega-Saturn-compiler-SH7604.cmake
    make

Contributing
============

Want to contribute? Great! I could always use some extra help!

### Found a bug or do you have a suggestion?

If so, I'd love to hear it. Create an issue [here][1].

[1]: https://github.com/ijacquez/libyaul/issues
[2]: http://www.youtube.com/mrkotfw

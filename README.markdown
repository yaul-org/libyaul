About
=====
  `libyaul` is an awesome open source SEGA Saturn software development kit that provides flexible low-level routines that are easily able to be composed to produce higher-level abstractions to further ease the burden of developing applications.

### Authors
 * Israel Jacquez <mrkotfw@gmail.com>

 For others, see the `CONTRIBUTORS` file.

### License
  To see the full text of the license, see the `LICENSE` file.

Usage
=====

### Requirements
 - CMake ver. 2.6 or greater (a Cross-Platform Makefile Generator)
 - GCC 4.6.3 or greater SH7604 cross-compiler tool-chain (see `tools/build-scripts/`)

### Installation
  If all requirements are met, as a _normal_ user and under the root of the `libyaul` source directory, perform the following to build the library

    mkdir -p build
    cd build/
    cp config.h.cmake{.in,}
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../SegaSaturn-compiler-SH7604.cmake
    make

Contributing
============

Want to contribute? Great! I could always use some extra help!

### Found a bug or do you have a suggestion?

If so, I'd love to hear it. Create an issue [here][1].

### Getting started

1. Fork it.
2. Create a branch (`git checkout -b 'libyaul-add-foo-bar'`)
3. Commit and changes (`git commit -am "Add foo bar"`)
4. When content with your work, push your changes (`git push origin 'libyaul-add-foo-bar'`)
4. Contact me with a link to your branch.
5. ???
6. Profit!

[1]: https://github.com/mrkotfw/libyaul/issues

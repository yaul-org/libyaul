About
=====
  A SEGA Saturn programming library that provides a _flexible_ and yet high-level library along with a suite of development tools to ease the burden in developing programs and games.

### Authors
 * Israel Jacques <[mrkotfw@eecs.berkeley.edu][1]>

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
    cmake .. -DCMAKE_TOOLCHAIN_FILE=../SegaSaturn-compiler-SH7604.cmake
    make

Contributing
============

Want to contribute? Great! I could always use some extra help!

### Found a bug or do you have a suggestion?

If so, I'd love to hear it. Create an issue [here][2].

### Getting started

1. Fork it.
2. Create a branch (`git checkout -b 'libyaul-add-foo-bar'`)
3. Commit and changes (`git commit -am "Add foo bar"`)
4. When content with your work, push your changes (`git push origin 'libyaul-add-foo-bar'`)
4. Contact me with a link to your branch.
5. ???
6. Profit!

[1]: mailto:mrkotfw@eecs.berkeley.edu
[2]: https://github.com/mrkotfw/libyaul/issues

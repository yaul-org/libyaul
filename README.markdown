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
 - GCC 4.6.3 or greater SH7604 cross-compiler tool-chain (see `tools/build-scripts/`)

### Installation
  If all requirements are met, as a _normal_ user and under the root of the `libyaul` source directory, perform the following to build the library

    cd libyaul

Initialize and update all submodules.

    git submodule init
    git submodule update -f

In the `yaul.env` you just copied, change the appropriate fields.

    cp -v yaul.env{.in,}
    $EDITOR yaul.env

Source `yaul.env' and build and install.

    make release
    make install-release
    make tools
    make install-tools

Contributing
============

Want to contribute? Great! I could always use some extra help!

### Found a bug or do you have a suggestion?

If so, I'd love to hear it. Create an issue [here][1].

### Rebranding/forking

If you're interested into further porting/adapting/whatever, please don't fork off. I hate it, and so does everyone else.

Let's try to keep everything under a single project.

[1]: https://github.com/ijacquez/libyaul/issues
[2]: http://www.youtube.com/mrkotfw

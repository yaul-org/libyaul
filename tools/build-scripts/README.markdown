Usage
=====

### Requirements
 - POSIX Bourne shell
 - GNU Wget
 - Time and especially patience

### Installation
  If all requirements are met, as a _normal_ user and under the root of the `tools/build-scripts` directory, perform the following to build the cross compiler

    cp -v config{.in,}

In the `config` you just copied, change the appropriate fields.

    emacs config

Before you begin building the tool-chain, be sure to double-check your changes in `config`. Otherwise,

    ./build-compiler

### Errors

   Be sure to check out the log files in the `${BUILD_SRC_DIR}/` directory.

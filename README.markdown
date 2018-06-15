<p align="center">
<img src="https://img.shields.io/github/license/mashape/apistatus.svg?maxAge=2592000" alt="license">
  <a href="https://travis-ci.org/ijacquez/libyaul">
    <img src="https://travis-ci.org/ijacquez/libyaul.svg?branch=master" alt="build status">
  </a>
</p>

Yaul
====
  Yaul is an open source SEGA Saturn development kit using the GNU tool-chain.

Installation
============
1. For Windows users without MSYS2 installed, download the installer from the [MSYS2][4] web page and follow the instructions [here][5].

2. A prebuilt tool-chain can either be downloaded from [yaul.org][2], or built from source.

   If downloading the prebuilt tool-chain, create a directory `tool-chain` and extract the contents of the archive into it. For Windows users, the `tool-chain` must reside under the MSYS2 system root path.

   If building from source, follow the instructions found in the `build-scripts` directory.

   For either of the two options, take note of the absolute path to the tool-chain.

3. Initialize and update all submodules.

       $ git submodule init
       $ git submodule update -f --recursive

4. Copy the template `yaul.env.in` to your home directory as `.yaul.env`. This is your environment file.

5. Open `.yaul.env` in a text editor and change the following to define your environment:
   1. Set the absolute path to the tool-chain in `INSTALL_ROOT`.
   2. Set the absolute path to where the `libyaul` source tree is located in `BUILD_ROOT`.
   3. Set the type of development cart you own in `OPTION_DEV_CARTRIDGE`. If none, set to 0 (zero).

   Setting the wrong values may result in compilation errors.

6. Read the environment file `.yaul.env` into your current shell.

       $ source ~/.yaul.env

7. Reading the environment file needs to be done every time a new shell is opened. To avoid having to do this every time, add the line below to your shell's startup file.

       $ echo 'source ~/.yaul.env' >> ~/.bash_profile

   If `.bash_profile` is not used, use `.profile` instead. This is dependent on your set up.

8. Build and install the supported libraries.

       $ SILENT=1 make install-release

   If any given library is being debugged, use the `install-debug` target instead. Either _release_ or _debug_ can currently be installed at one time. It's possible to switch between the two in the same installation.

   To find more about other targets, call `make list-targets`.

9. Build and install the tools.

       $ SILENT=1 make install-tools

10. You can now build any of the given examples in the `libyaul-examples` submodule.

Contributing
============
Want to contribute? Great! I could always use some extra help! Found a bug or do you have a suggestion? If so, I'd love to hear it. Create an issue [here][1].

Contact
=======
**#yabause** on freenode ([irc.freenode.net][3]).

[1]: https://github.com/ijacquez/libyaul/issues
[2]: http://yaul.org/
[3]: https://freenode.net/
[4]: https://www.msys2.org/
[5]: https://github.com/msys2/msys2/wiki/MSYS2-installation

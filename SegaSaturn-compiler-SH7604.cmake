INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME SegaSaturn)
SET(CMAKE_SYSTEM_PROCESSOR SH7604)

# Specify the cross compiler.
CMAKE_FORCE_C_COMPILER(sh-elf-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(sh-elf-g++ GNU)

# Where is the target environment. This can be changed by user.
SET(CMAKE_FIND_ROOT_PATH /usr/local/share/sh-elf/bin)

# Search for programs in the build host directories.
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# For libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# These settings must be set in order to build the library.
IF(CMAKE_COMPILER_IS_GNUCC)
  SET_SOURCE_FILES_PROPERTIES(${SRC} COMPILE_FLAGS "-m2 -mb")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

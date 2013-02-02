SET(SRC_DIRS
  common
  common/gdb
  cons
  kernel
  kernel/lib
  kernel/mm
  kernel/vfs
  kernel/vfs/fs/iso9660
  kernel/vfs/fs/romdisk
  newlib
  scu
  scu/bus/a/cs0/dram-cartridge
  scu/bus/a/cs0/usb-cartridge
  scu/bus/a/cs2/cd-block
  scu/bus/b/scsp
  scu/bus/b/vdp1
  scu/bus/b/vdp2
  scu/bus/cpu
  scu/bus/cpu/smpc
)

SET(INCLUDE_DIRS
  .
  common
  common/gdb
  cons
  kernel
  scu
  scu/bus/a/cs0/dram-cartridge
  scu/bus/a/cs0/usb-cartridge
  scu/bus/a/cs2/cd-block
  scu/bus/b/scsp
  scu/bus/b/vdp1
  scu/bus/b/vdp2
  scu/bus/cpu
  scu/bus/cpu/smpc
)

# Macro that prepends a given string onto the beginning of each string in a list.
MACRO(PREPEND _LIST _STR)
  SET(_TMP_LIST ${${_LIST}})
  SET(${_LIST})
  FOREACH(X ${_TMP_LIST})
    SET(${_LIST} ${${_LIST}} ${_STR}${X})
  ENDFOREACH(X)
ENDMACRO(PREPEND)

# Macro that replaces slashes and spaces in a string with underscores.
MACRO(SANITIZE_PATH _STR)
  STRING(REGEX REPLACE "[\\/ ]+" "_" ${_STR} ${${_STR}})
ENDMACRO(SANITIZE_PATH)

# Append a list of all object files.
FOREACH(DIR_LIST ${SRC_DIRS})
  SET(_TMP_DIR_LIST "${DIR_LIST}")

  MESSAGE(STATUS "Processing ${CMAKE_CURRENT_SOURCE_DIR}/${DIR_LIST}/CMakeLists.txt")

  INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/${DIR_LIST}/CMakeLists.txt)

  SANITIZE_PATH(DIR_LIST)
  PREPEND(${DIR_LIST}_SRC "${_TMP_DIR_LIST}/")
  PREPEND(${DIR_LIST}_INSTALL_FILES "${_TMP_DIR_LIST}/")

  SET(SRC ${SRC} ${${DIR_LIST}_SRC})
  SET(INSTALL_FILES ${INSTALL_FILES} ${${DIR_LIST}_INSTALL_FILES})
ENDFOREACH(DIR_LIST)

# Build options for library.
OPTION(WITH_DEBUG "Build the project using debugging code" OFF)

IF(CMAKE_COMPILER_IS_GNUCC)
  SET_SOURCE_FILES_PROPERTIES(${SRC} PROPERTIES LANGUAGE C COMPILE_FLAGS "-g -ansi -std=c99 -pedantic -W -Wall -Wshadow -Werror -Wunused -O2 -fomit-frame-pointer -ffast-math -fstrict-aliasing")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(WITH_DEBUG)
  ADD_DEFINITIONS(-DDEBUG)
ENDIF(WITH_DEBUG)

# Default include directories.
INCLUDE_DIRECTORIES(AFTER ${INCLUDE_DIRS})

# Build the library.
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRC})

INSTALL(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib)

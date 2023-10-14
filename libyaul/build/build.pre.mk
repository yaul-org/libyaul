ifeq ($(strip $(YAUL_INSTALL_ROOT)),)
  $(error Undefined YAUL_INSTALL_ROOT (install root directory))
endif

ifneq (1,$(words [$(strip $(YAUL_INSTALL_ROOT))]))
  $(error YAUL_INSTALL_ROOT (install root directory) contains spaces)
endif

ifeq ($(strip $(YAUL_ARCH_SH_PREFIX)),)
  $(error Undefined YAUL_ARCH_SH_PREFIX (tool-chain prefix))
endif

ifneq (1,$(words [$(strip $(YAUL_ARCH_SH_PREFIX))]))
  $(error YAUL_ARCH_SH_PREFIX (tool-chain prefix) contains spaces)
endif

ifneq (1,$(words [$(strip $(YAUL_PROG_SH_PREFIX))]))
  $(error YAUL_PROG_SH_PREFIX (tool-chain program prefix) contains spaces)
endif

ifeq ($(strip $(SILENT)),)
  ECHO=
else
  ECHO=@
endif
export ECHO

ifeq ($(strip $(NOCOLOR)),)
V_BEGIN_BLACK= [1;30m
V_BEGIN_RED= [1;31m
V_BEGIN_GREEN= [1;32m
V_BEGIN_YELLOW= [1;33m
V_BEGIN_BLUE= [1;34m
V_BEGIN_MAGENTA= [1;35m
V_BEGIN_CYAN= [1;36m
V_BEGIN_WHITE= [1;37m
V_END= [m
else
V_BEGIN_BLACK=
V_BEGIN_RED=
V_BEGIN_GREEN=
V_BEGIN_YELLOW=
V_BEGIN_BLUE=
V_BEGIN_MAGENTA=
V_BEGIN_CYAN=
V_BEGIN_WHITE=
V_END=
endif
export V_BEGIN_YELLOW

# Get the Makefile that calls to include this Makefile
BUILD_ROOT:= $(patsubst %/,%,$(dir $(abspath $(firstword $(MAKEFILE_LIST)))))

ifeq '$(OS)' "Windows_NT"
EXE_EXT:= .exe

# In order to avoid the following error under MSYS2 and MinGW-w32:
#
# /opt/tool-chains/sh2eb-elf/bin/sh2eb-elf-gcc.exe: error while loading shared
# libraries: libwinpthread-1.dll: cannot open shared object file: No such file
# or directory
#
# We need to have /mingw/bin in our path. It's unclear exactly why, but
# libwinpthread-1.dll resides in /mingw64/bin. Copying libwinpthread-1.dll to
# /opt/tool-chains/sh2eb-elf/bin does not resolve the issue
PATH:= /mingw64/bin:$(PATH)
endif

# $1 -> Relative/absolute path to be converted using '@' instead of '/'
define macro-convert-build-path
$(SH_BUILD_PATH)/$(subst /,@,$(abspath $1))
endef

# $1 -> Variable
# $2 -> Index
define macro-word-split
$(word $2,$(subst ;, ,$1))
endef

# Always include Yaul
include $(YAUL_INSTALL_ROOT)/share/build.yaul.mk

# Customizable (must be overwritten in user's Makefile)
SH_PROGRAM?= unknown-program
SH_DEFSYMS?=
SH_SRCS?=
SH_SRCS_NO_LINK?=
SH_BUILD_DIR?= build
SH_OUTPUT_DIR?= .

# Customizable variables (must be overwritten in user's Makefile)
#   IMAGE_DIRECTORY        ISO/CUE
#   AUDIO_TRACKS_DIRECTORY ISO/CUE
#   IMAGE_1ST_READ_BIN     ISO/CUE
#   IP_VERSION             ISO/CUE, SS
#   IP_RELEASE_DATE        ISO/CUE, SS
#   IP_AREAS               ISO/CUE, SS
#   IP_PERIPHERALS         ISO/CUE, SS
#   IP_TITLE               ISO/CUE, SS
#   IP_MASTER_STACK_ADDR   ISO/CUE, SS
#   IP_SLAVE_STACK_ADDR    ISO/CUE, SS
#   IP_1ST_READ_ADDR       ISO/CUE, SS
#   IP_1ST_READ_SIZE       ISO/CUE, SS

YAUL_PROG_SH_PREFIX?= $(YAUL_ARCH_SH_PREFIX)
ifeq ($(strip $(YAUL_PROG_SH_PREFIX)),)
YAUL_PROG_SH_PREFIX:= $(YAUL_ARCH_SH_PREFIX)
endif

SH_AS:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-as$(EXE_EXT)
SH_AR:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-gcc-ar$(EXE_EXT)
SH_CC:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-gcc$(EXE_EXT)
SH_CXX:=     $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-g++$(EXE_EXT)
SH_LD:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-gcc$(EXE_EXT)
SH_NM:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-gcc-nm$(EXE_EXT)
SH_OBJCOPY:= $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-objcopy$(EXE_EXT)
SH_OBJDUMP:= $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-objdump$(EXE_EXT)

M68K_AS:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_ARCH_M68K_PREFIX)-as$(EXE_EXT)
M68K_AR:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_ARCH_M68K_PREFIX)-gcc-ar$(EXE_EXT)
M68K_CC:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_ARCH_M68K_PREFIX)-gcc$(EXE_EXT)
M68K_CXX:=     $(YAUL_INSTALL_ROOT)/bin/$(YAUL_ARCH_M68K_PREFIX)-g++$(EXE_EXT)
M68K_LD:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_ARCH_M68K_PREFIX)-gcc$(EXE_EXT)
M68K_NM:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_ARCH_M68K_PREFIX)-gcc-nm$(EXE_EXT)
M68K_OBJCOPY:= $(YAUL_INSTALL_ROOT)/bin/$(YAUL_ARCH_M68K_PREFIX)-objcopy$(EXE_EXT)
M68K_OBJDUMP:= $(YAUL_INSTALL_ROOT)/bin/$(YAUL_ARCH_M68K_PREFIX)-objdump$(EXE_EXT)

SH_AFLAGS= --fatal-warnings
SH_CFLAGS_shared:= \
	-W \
	-Wall \
	-Wduplicated-branches \
	-Wduplicated-cond \
	-Wextra \
	-Winit-self \
	-Wmissing-include-dirs \
	-Wno-format \
	-Wno-main \
	-Wnull-dereference \
	-Wshadow \
	-Wstrict-aliasing \
	-Wunused \
	-Wunused-parameter \
	-save-temps=obj

SH_LDFLAGS:= \
	-static \
	-Wl,--gc-sections \
	-Wl,-Map,$(SH_BUILD_PATH)/$(SH_PROGRAM).map \
	$(YAUL_LDFLAGS)

SH_CFLAGS:= \
	-std=c11 \
	-Wbad-function-cast \
	$(SH_CFLAGS_shared) \
	$(YAUL_CFLAGS)

SH_CXXFLAGS:= \
	-std=c++17 \
	-fno-exceptions \
	-fno-rtti \
	-fno-unwind-tables \
	-fno-asynchronous-unwind-tables \
	-fno-threadsafe-statics \
	-fno-use-cxa-atexit \
	$(SH_CFLAGS_shared) \
	$(YAUL_CXXFLAGS)

SH_BUILD_PATH= $(abspath $(SH_BUILD_DIR))
SH_OUTPUT_PATH= $(abspath $(SH_OUTPUT_DIR))

.PHONY: all generate-cdb clean .build

.SUFFIXES:
.SUFFIXES: .c .cc .C .cpp .cxx .sx .o .bin .elf

.PRECIOUS: %.elf %.c %.o

.SECONDARY: .build

all: .build

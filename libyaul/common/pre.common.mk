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

ifeq ($(strip $(YAUL_CDB)),)
  $(error Undefined YAUL_CDB (update JSON compile command database))
endif

# Check options
ifeq ($(strip $(YAUL_OPTION_DEV_CARTRIDGE)),)
  $(error Undefined YAUL_OPTION_DEV_CARTRIDGE (development cartridge option))
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
endif

# Customizable (must be overwritten in user's Makefile)
SH_PROGRAM?= unknown-program
SH_OBJECTS?=
SH_OBJECTS_NO_LINK?=
SH_LIBRARIES?=
SH_CUSTOM_SPECS?=
M68K_PROGRAM?= undefined-program
M68K_OBJECTS?=
ROMDISK_DEPS?=
IMAGE_DIRECTORY?= cd
IMAGE_1ST_READ_BIN?= A.BIN

YAUL_PROG_SH_PREFIX?= $(YAUL_ARCH_SH_PREFIX)
ifeq ($(strip $(YAUL_PROG_SH_PREFIX)),)
YAUL_PROG_SH_PREFIX:= $(YAUL_ARCH_SH_PREFIX)
endif

SH_AS:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-as$(EXE_EXT)
SH_AR:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-ar$(EXE_EXT)
SH_CC:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-gcc$(EXE_EXT)
SH_CXX:=     $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-g++$(EXE_EXT)
SH_LD:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-gcc$(EXE_EXT)
SH_NM:=      $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-nm$(EXE_EXT)
SH_OBJCOPY:= $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-objcopy$(EXE_EXT)
SH_OBJDUMP:= $(YAUL_INSTALL_ROOT)/bin/$(YAUL_PROG_SH_PREFIX)-objdump$(EXE_EXT)

M68K_ARCH:= m68k-elf

M68K_AS:= $(YAUL_INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-as$(EXE_EXT)
M68K_AR:= $(YAUL_INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-ar$(EXE_EXT)
M68K_CC:= $(YAUL_INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-gcc$(EXE_EXT)
M68K_CXX:= $(YAUL_INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-g++$(EXE_EXT)
M68K_LD:= $(YAUL_INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-gcc$(EXE_EXT)
M68K_NM:= $(YAUL_INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-nm$(EXE_EXT)
M68K_OBJCOPY:= $(YAUL_INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-objcopy$(EXE_EXT)
M68K_OBJDUMP:= $(YAUL_INSTALL_ROOT)/$(M68K_ARCH)/bin/$(M68K_ARCH)-objdump$(EXE_EXT)

SH_AFLAGS= --fatal-warnings
SH_CFLAGS= \
	-W \
	-Wall \
	-Wextra \
	-Werror \
	-Wunused-parameter \
	-Wstrict-aliasing \
	-Wno-main \
	-Wno-format \
	-DHAVE_DEV_CARTRIDGE=$(YAUL_OPTION_DEV_CARTRIDGE) \
	-DHAVE_GDB_SUPPORT=$(YAUL_OPTION_BUILD_GDB)

SH_LDFLAGS= \
	-Wl,-Map,$(SH_PROGRAM).map

SH_CXXFLAGS= $(SH_CFLAGS)
SH_LXXFLAGS= $(SH_LDFLAGS)

M68K_AFLAGS= --fatal-warnings
M68K_LDFLAGS=

IP_VERSION?= V1.000
IP_RELEASE_DATE?= YYYYMMDD
IP_AREAS?= JTUBKAEL
IP_PERIPHERALS?= JAMKST
IP_TITLE?= Title
IP_MASTER_STACK_ADDR?= 0x06004000
IP_SLAVE_STACK_ADDR?= 0x06002000
IP_1ST_READ_ADDR?= 0x06004000
IP_1ST_READ_SIZE?= 0

ROMDISK_FLAGS:= -a 16 -V "ROOT"
MAKE_ISO_REDIRECT:= >/dev/null 2>&1

ifeq ($(strip $(SILENT)),)
  ROMDISK_FLAGS:= -v $(ROMDISK_FLAGS)
  MAKE_ISO_REDIRECT:=
endif

SUFFIXES:= .c .cc .C .cpp .cxx .m68k.sx .sx .o .m68k .bin .elf .romdisk .romdisk.o

.PHONY: all clean example list-targets

.SUFFIXES:
.SUFFIXES: $(SUFFIXES)

.PRECIOUS: %.elf %.c %.o

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

# Check options
ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),)
  $(error Undefined OPTION_DEV_CARTRIDGE (development cartridge option))
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

SH_AS:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-as
SH_AR:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-ar
SH_CC:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-gcc
SH_CXX:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-g++
SH_LD:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-gcc
SH_NM:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-nm
SH_OBJCOPY:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-objcopy
SH_OBJDUMP:= $(INSTALL_ROOT)/sh-elf/bin/sh-elf-objdump
M68K_AS:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-as
M68K_AR:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-ar
M68K_LD:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-ld
M68K_NM:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-nm
M68K_OBJCOPY:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objcopy
M68K_OBJDUMP:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objdump

SH_AFLAGS= --fatal-warnings
SH_CFLAGS= \
	-W \
	-Wall \
	-Wextra \
	-Werror \
	-Wunused-parameter \
	-Wstrict-aliasing \
	-Wno-main \
	-O2 \
	-DHAVE_DEV_CARTRIDGE=$(OPTION_DEV_CARTRIDGE) \
	-DFIXMATH_NO_OVERFLOW=1 \
	-DFIXMATH_NO_ROUNDING=1 \
	-save-temps
SH_LDFLAGS= -Wl,-Map,$(SH_PROGRAM).map

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

SUFFIXES:= .c .cc .C .cpp .cxx .m68k.sx .sx .o .m68k .bin .elf .romdisk .romdisk.o

.PHONY: all clean example list-targets

.SUFFIXES:
.SUFFIXES: $(SUFFIXES)

.PRECIOUS: %.elf %.c %.o

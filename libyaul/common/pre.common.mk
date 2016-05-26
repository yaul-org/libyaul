ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

# Check options
ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),)
  $(error Undefined OPTION_DEV_CARTRIDGE (development cartridge option))
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
M68K_CC:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-gcc
M68K_CXX:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-g++
M68K_LD:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-gcc
M68K_NM:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-nm
M68K_OBJCOPY:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objcopy
M68K_OBJDUMP:= $(INSTALL_ROOT)/m68k-elf/bin/m68k-elf-objdump

SH_AFLAGS= --fatal-warnings --isa=sh2 --big --reduce-memory-overheads
SH_CFLAGS= -W -Wall -Wextra -Werror -Wunused-parameter -Wstrict-aliasing -Wno-main \
	-m2 -mb -O2 -fomit-frame-pointer \
	-ffast-math -fstrict-aliasing \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/tga \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/common \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/cons \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/math \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/lib \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/fs/iso9660 \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/fs/romdisk \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/a/cs0/arp \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/a/cs0/dram-cartridge \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/a/cs0/usb-cartridge \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/a/cs2/cd-block \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/b/scsp \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/b/vdp1 \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/b/vdp2 \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/cpu \
	-I$(INSTALL_ROOT)/sh-elf/sh-elf/include/yaul/scu/bus/cpu/smpc \
	-DHAVE_DEV_CARTRIDGE=$(OPTION_DEV_CARTRIDGE) \
	-DFIXMATH_NO_OVERFLOW=1 \
	-DFIXMATH_NO_ROUNDING=1
SH_LDFLAGS= -Wl,-Map,$(SH_PROGRAM).map

SH_CXXFLAGS= $(SH_CFLAGS) \
	-ffreestanding -fno-exceptions -fno-rtti
SH_LXXFLAGS= -Wl,-Map,$(SH_PROGRAM).map \
	-Wl,--start-group \
	-lstdc++ -lc -lgcc \
	-Wl,--end-group \
	-nostartfiles -specs=yaul.specs

M68K_AFLAGS= --fatal-warnings
M68K_CFLAGS=
M68K_LDFLAGS=

SUFFIXES:= .c .m68k.S .S .o .m68k .bin .elf .romdisk .romdisk.o

.PHONY: all clean example list-targets

.SUFFIXES:
.SUFFIXES: $(SUFFIXES)

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

# Check options
ifeq ($(strip $(OPTION_DEV_CARTRIDGE)),)
  $(error Undefined OPTION_DEV_CARTRIDGE (development cartridge option))
endif

# Customizable (must be overwritten in user's Makefile)
PROJECT?=unknown
OBJECTS?=
OBJECTS_NO_LINK?=
CUSTOM_SPECS?=
IMAGE_DIRECTORY?= cd
IMAGE_1ST_READ_BIN?= A.BIN
ARCH?= sh-elf

AS:= $(INSTALL_ROOT)/bin/$(ARCH)-as
AR:= $(INSTALL_ROOT)/bin/$(ARCH)-ar
CC:= $(INSTALL_ROOT)/bin/$(ARCH)-gcc
CXX:= $(INSTALL_ROOT)/bin/$(ARCH)-g++
LD:= $(INSTALL_ROOT)/bin/$(ARCH)-gcc
NM:= $(INSTALL_ROOT)/bin/$(ARCH)-nm
OBJCOPY:= $(INSTALL_ROOT)/bin/$(ARCH)-objcopy
OBJDUMP:= $(INSTALL_ROOT)/bin/$(ARCH)-objdump

AFLAGS= --fatal-warnings --isa=sh2 --big --reduce-memory-overheads
CFLAGS= -W -Wall -Wextra -Werror -Wunused-parameter -Wstrict-aliasing -Wno-main \
	-m2 -mb -O2 -fomit-frame-pointer \
	-ffast-math -fstrict-aliasing \
	-I$(INSTALL_ROOT)/$(ARCH)/include/fixmath \
	-I$(INSTALL_ROOT)/$(ARCH)/include/tga \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/common \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/cons \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/math \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/lib \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/fs/iso9660 \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/fs/romdisk \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/a/cs0/arp \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/a/cs0/dram-cartridge \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/a/cs0/usb-cartridge \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/a/cs2/cd-block \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/b/scsp \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/b/vdp1 \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/b/vdp2 \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/cpu \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/cpu/smpc \
	-DHAVE_DEV_CARTRIDGE=$(OPTION_DEV_CARTRIDGE)
LDFLAGS= -Wl,-Map,$(PROJECT).map

CXXFLAGS= $(CFLAGS) \
	-ffreestanding -fno-exceptions -fno-rtti
LXXFLAGS= -Wl,-Map,$(PROJECT).map \
	-Wl,--start-group \
	-lstdc++ -lc -lgcc \
	-Wl,--end-group \
	-nostartfiles -specs=yaul.specs

SUFFIXES:= .c .S .o .bin .elf .romdisk .romdisk.o

.PHONY: clean example romdisk image

.SUFFIXES:
.SUFFIXES: $(SUFFIXES)

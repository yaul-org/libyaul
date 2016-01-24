ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

PROJECT?=unknown

ARCH?= sh-elf

CXX:= $(ARCH)-g++
AS:= $(ARCH)-as
AR:= $(ARCH)-ar
CC:= $(ARCH)-gcc
LD:= $(ARCH)-gcc
NM:= $(ARCH)-nm
OBJCOPY:= $(ARCH)-objcopy
OBJDUMP:= $(ARCH)-objdump

AFLAGS= --fatal-warnings --isa=sh2 --big --reduce-memory-overheads
CFLAGS= -W -Wall -Wextra -Werror -Wunused-parameter -Wstrict-aliasing -Wno-main \
	-m2 -mb -O2 -fomit-frame-pointer \
	-ffast-math -fstrict-aliasing \
	-I$(INSTALL_ROOT)/$(ARCH)/include/fixmath \
	-I$(INSTALL_ROOT)/$(ARCH)/include/tga \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/common \
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/cons \
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
	-I$(INSTALL_ROOT)/$(ARCH)/include/yaul/scu/bus/cpu/smpc
LDFLAGS= -Wl,-Map,${PROJECT}.map -specs=yaul.specs

CXXFLAGS= $(CFLAGS) \
	-ffreestanding -fno-exceptions -fno-rtti
LXXFLAGS= -Wl,-Map,${PROJECT}.map \
	-Wl,--start-group \
	-lstdc++ -lc -lgcc \
	-Wl,--end-group \
	-nostartfiles -specs=yaul.specs

OBJECTS:=

SUFFIXES:= .c .S .o .bin .elf .romdisk .romdisk.o

.PHONY: clean example romdisk image

.SUFFIXES:
.SUFFIXES: $(SUFFIXES)

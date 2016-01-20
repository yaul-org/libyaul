ROOT:= $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

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
	-I$(ROOT)/../libtga \
	-I$(ROOT)/../libfixmath \
	-I$(ROOT)/../libyaul \
	-I$(ROOT)/../libyaul/common \
	-I$(ROOT)/../libyaul/common/gdb \
	-I$(ROOT)/../libyaul/cons \
	-I$(ROOT)/../libyaul/kernel \
	-I$(ROOT)/../libyaul/kernel/vfs \
	-I$(ROOT)/../libyaul/scu \
	-I$(ROOT)/../libyaul/scu/bus/a/cs0/arp \
	-I$(ROOT)/../libyaul/scu/bus/a/cs0/dram-cartridge \
	-I$(ROOT)/../libyaul/scu/bus/a/cs0/usb-cartridge \
	-I$(ROOT)/../libyaul/scu/bus/a/cs2/cd-block \
	-I$(ROOT)/../libyaul/scu/bus/b/scsp \
	-I$(ROOT)/../libyaul/scu/bus/b/vdp1 \
	-I$(ROOT)/../libyaul/scu/bus/b/vdp2 \
	-I$(ROOT)/../libyaul/scu/bus/cpu \
	-I$(ROOT)/../libyaul/scu/bus/cpu/smpc
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

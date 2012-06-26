ROOTDIR:= $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))
CC_PREFIX?= sh-elf

AS= $(CC_PREFIX)-as
CC= $(CC_PREFIX)-gcc
LD= $(CC_PREFIX)-gcc
NM= $(CC_PREFIX)-nm
OB= $(CC_PREFIX)-objcopy
OD= $(CC_PREFIX)-objdump

AFLAGS= --fatal-warnings --isa=sh2 --big --reduce-memory-overheads
CFLAGS= -W -Wall -Wextra -Werror -Wshadow -Wunused-parameter \
	-ansi -m2 -mb -O2 -fomit-frame-pointer \
	-ffast-math -fstrict-aliasing \
	-I../../libyaul/common \
	-I../../libyaul/cons \
	-I../../libyaul/fs \
	-I../../libyaul/scu \
	-I../../libyaul/scu/bus/a/cs0 \
	-I../../libyaul/scu/bus/a/cs0/arp \
	-I../../libyaul/scu/bus/a/cs0/dram-cartridge \
	-I../../libyaul/scu/bus/a/cs0/usb-cartridge \
	-I../../libyaul/scu/bus/a/cs1 \
	-I../../libyaul/scu/bus/a/cs2 \
	-I../../libyaul/scu/bus/b/scsp \
	-I../../libyaul/scu/bus/b/vdp1 \
	-I../../libyaul/scu/bus/b/vdp2 \
	-I../../libyaul/scu/bus/cpu \
	-I../../libyaul/scu/bus/cpu/smpc
LDFLAGS= -Wl,-Map,${PROJECT}.map \
	-L$(ROOTDIR)/../build/libyaul \
	-Wl,--start-group -lyaul -lc -lgcc \
	-Wl,--end-group \
	-Wl,--gc-sections \
	-nostartfiles -T $(ROOTDIR)/common/ldscripts/sh-elf.x

# All programs must link this as the first object (crt0.o)
OBJECTS:= $(ROOTDIR)/common/crt0.o \
	$(ROOTDIR)/common/crt0-init.o

all: example

bootstrap: bootstrap.o
	$(CC) -nostdlib -m2 -mb -nostartfiles \
		-T $(ROOTDIR)/common/ldscripts/bootstrap.x \
		$< -o $@

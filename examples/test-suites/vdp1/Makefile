ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

include $(INSTALL_ROOT)/share/pre.common.mk

SH_PROGRAM:= vdp1
SH_OBJECTS:= \
	root.romdisk.o \
	fs.o \
	globals.o \
	state.o \
	state_machine.o \
	test.o \
	tests.o \
	vdp1.o \
	tests/primitives/normal_sprite/00.o \
	tests/primitives/scaled_sprite/00.o \
	tests/primitives/scaled_sprite/01.o
SH_LIBRARIES:= tga
SH_CFLAGS+= -I.

IP_VERSION:= V1.000
IP_RELEASE_DATE:= 20160101
IP_AREAS:= JTUBKAEL
IP_PERIPHERALS:= JAMKST
IP_TITLE:= vdp1
IP_MASTER_STACK_ADDR:= 0x06100000
IP_SLAVE_STACK_ADDR:= 0x00000000
IP_1ST_READ_ADDR:= 0x06004000

include $(INSTALL_ROOT)/share/post.common.mk

ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

include $(INSTALL_ROOT)/share/pre.common.mk

SH_PROGRAM:= nbgs
SH_OBJECTS:= \
	root.romdisk.o \
	fs.o \
	nbgs.o

IP_VERSION:= V1.000
IP_RELEASE_DATE:= 20160101
IP_AREAS:= JTUBKAEL
IP_PERIPHERALS:= JAMKST
IP_TITLE:= romdisk
IP_MASTER_STACK_ADDR:= 0x06100000
IP_SLAVE_STACK_ADDR:= 0x00000000
IP_1ST_READ_ADDR:= 0x06004000

M68K_PROGRAM:= out.m68k
M68K_OBJECTS:= out.m68k.o

include $(INSTALL_ROOT)/share/post.common.mk

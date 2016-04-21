ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

include $(INSTALL_ROOT)/share/pre.common.mk

PROJECT:= inkfill

IP_VERSION:= V1.000
IP_RELEASE_DATE:= 20160101
IP_AREAS:= JTUBKAEL
IP_PERIPHERALS:= JAMKST
IP_TITLE:= Ink Fill!
IP_MASTER_STACK_ADDR:= 0x06100000
IP_SLAVE_STACK_ADDR:= 0x00000000
IP_1ST_READ_ADDR:= 0x06004000

LIBRARIES:= tga
OBJECTS:= \
	root.romdisk.o \
	fs.o \
	game-over.o \
	game.o \
	globals.o \
	inkfill.o \
	scene.o \
	title.o \
	tutorial.o

include $(INSTALL_ROOT)/share/post.common.mk

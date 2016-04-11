ifeq ($(strip $(INSTALL_ROOT)),)
  $(error Undefined INSTALL_ROOT (install root directory))
endif

include $(INSTALL_ROOT)/share/yaul/pre.common.mk

PROJECT:= dark-blue

IP_VERSION:= V1.000
IP_RELEASE_DATE:= 20160101
IP_AREAS:= JTUBKAEL
IP_PERIPHERALS:= JAMKST
IP_TITLE:= Dark Blue
IP_MASTER_STACK_ADDR:= 0x06100000
IP_SLAVE_STACK_ADDR:= 0x00000000
IP_1ST_READ_ADDR:= 0x06004000

LIBRARIES:= tga fixmath
OBJECTS:= \
	root.romdisk.o \
	cmd_groups.o \
	collider.o \
	dark-blue.o \
	fs.o \
	fs_texture.o \
	globals.o \
	object_camera.o \
	object_fade.o \
	object_player.o \
	object_world.o \
	physics.o \
	rigid_body.o \
	state.o \
	state_game.o \
	state_machine.o \
	state_splash.o \
	state_title.o

ROMDISK_DEPS:= levels/test.tmx levels/test.json romdisk/*.TGA

.SUFFIXES:
.SUFFIXES: $(SUFFIXES) .tmx .json

%.json: %.tmx
	if ! which tiled >/dev/null 2>&1; then \
	    printf -- "Tiled Map Editor (tiled) is required (http://www.mapeditor.org/)\n" >&2; \
	    exit 1; \
	fi
	echo tiled --export-map "Json files (*.json)" $< $@
	tools/tmx2map
	echo $(RM) $@

include $(INSTALL_ROOT)/share/yaul/post.common.mk

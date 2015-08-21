include ../../pre.common.mk

PROJECT:= dark-blue
OBJECTS+= \
	root.romdisk.o \
	camera.o \
	cmd_groups.o \
	collider.o \
	dark-blue.o \
	fs.o \
	globals.o \
	physics.o \
	player.o \
	rigid_body.o \
	state.o \
	state_game.o \
	state_machine.o \
	world.o

ROMDISK_DEPS:= levels/test.json

.SUFFIXES:
.SUFFIXES: $(SUFFIXES) .tmx .json

%.json: %.tmx
	tiled --export-map "Json files (*.json)" $< $@
	tools/tmx2map
	rm -f $@

include ../../post.common.mk

include ../pre.common.mk

PROJECT:= game-dark-blue
OBJECTS+= \
	root.romdisk.o \
	fs.o \
	state_machine.o \
	state.o \
	state_game.o \
	globals.o \
	player.o \
	world.o \
	camera.o \
	physics.o \
	dark-blue.o

ROMDISK_DEPS:= levels/test.json

.SUFFIXES:
.SUFFIXES: $(SUFFIXES) .tmx .json

%.json: %.tmx
	tiled --export-map "Json files (*.json)" $< $@
	tools/tmx2map
	rm -f $@

include ../post.common.mk

# -*- mode: makefile -*-

LIB_SRCS:= \
	camera.c \
	gst.c \
	light.c \
	mic3d.c \
	perf.c \
	render.c \
	sort.c \
	state.c \
	tlist.c

INSTALL_HEADER_FILES:= \
	./mic3d/:config.h:./mic3d/ \
	./mic3d/:sizes.h:./mic3d/ \
	./mic3d/:types.h:./mic3d/ \
	./mic3d/:workarea.h:./mic3d/ \
	./:mic3d.h:./ \

# -*- mode: makefile -*-

LIB_SRCS:= \
	camera.c \
	gst.c \
	light.c \
	list.c \
	matrix.c \
	mic3d.c \
	perf.c \
	render.c \
	sort.c \
	state.c \
	tlist.c \
	workarea.c

INSTALL_HEADER_FILES:= \
	./mic3d/:types.h:./mic3d/ \
	./:mic3d.h:./ \

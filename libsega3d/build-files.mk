# -*- mode: makefile -*-

LIB_SRCS:= \
	fog.c \
	list.c \
	matrix_stack.c \
	perf.c \
	plist.c \
	plist.c \
	s3d.c \
	sega3d.c \
	sort.c \
	state.c \
	tlist.c \
	transform.c

INSTALL_HEADER_FILES:= \
	./sega3d/:perf.h:./sega3d/ \
	./sega3d/:s3d.h:./sega3d/ \
	./sega3d/:types.h:./sega3d/ \
	./sega3d/:sgl.h:./sega3d/ \
	./:sega3d.h:./sega3d/ \

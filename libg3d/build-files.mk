# -*- mode: makefile -*-

LIB_SRCS:= \
	fog.c \
	list.c \
	matrix_stack.c \
	perf.c \
	plist.c \
	plist.c \
	s3d.c \
	g3d.c \
	sort.c \
	state.c \
	tlist.c \
	transform.c

INSTALL_HEADER_FILES:= \
	./g3d/:perf.h:./g3d/ \
	./g3d/:s3d.h:./g3d/ \
	./g3d/:types.h:./g3d/ \
	./g3d/:sgl.h:./g3d/ \
	./:g3d.h:./g3d/ \

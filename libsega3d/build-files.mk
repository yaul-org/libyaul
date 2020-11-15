# -*- mode: makefile -*-

LIB_SRCS:= \
	state.c \
	sega3d.c \
	tlist.c \
	transform.c \
	sort.c \
	matrix_stack.c \
	fog.c \
	vertex_pool_transform.sx

INSTALL_HEADER_FILES:= \
	./:sega3d.h:./sega3d/ \
	./:sgl.h:./sega3d/

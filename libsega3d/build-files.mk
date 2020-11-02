# -*- mode: makefile -*-

LIB_SRCS:= sega3d.c tlist.c sort.c matrix_stack.c

INSTALL_HEADER_FILES:= \
	./:sega3d.h:./sega3d/ \
	./:sgl.h:./sega3d/

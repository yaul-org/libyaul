# -*- mode: makefile -*-

LIB_SRCS:= sega3d.c tlist.c sort.c matrix_stack.c vertex_pool_transform.sx

INSTALL_HEADER_FILES:= \
	./:sega3d.h:./sega3d/ \
	./:sgl.h:./sega3d/

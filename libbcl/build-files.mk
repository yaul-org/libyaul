# -*- mode: makefile -*-

LIB_SRCS:= huffman.c \
	lz.c \
	prs.c \
	rice.c \
	rle.c \
	shannonfano.c

INSTALL_HEADER_FILES:= \
	./:bcl.h:./bcl/

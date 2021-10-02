# -*- mode: makefile -*-

LIB_SRCS:= huffman.c \
	lz.c \
	prs.c \
	rle.c

INSTALL_HEADER_FILES:= \
	./:bcl.h:./bcl/

# -*- mode: makefile -*-

LIB_SRCS:= huffman.c \
	lz.c \
	prs.c \
	rice.c \
	rle.c \
	shannonfano.c

INCLUDE_DIRS:= \
	../lib$(MAIN_TARGET)/lib/lib

INSTALL_HEADER_FILES:= \
	./:bcl.h:./bcl/

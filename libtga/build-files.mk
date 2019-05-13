# -*- mode: makefile -*-

LIB_SRCS:= tga.c

INCLUDE_DIRS:= \
	../lib$(MAIN_TARGET)/lib/lib

INSTALL_HEADER_FILES:= \
	./:tga.h:./tga/
